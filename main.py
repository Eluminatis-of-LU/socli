import urllib.parse
import keyring
import httpx
import click
from keyrings.cryptfile.cryptfile import CryptFileKeyring
import dotenv
import sys
import os
import random
import string
import functools

TARGET_URL = (
    dotenv.get_key(".env", "TARGET_URL") or "https://judge.eluminatis-of-lu.com"
)

client = httpx.Client(base_url=TARGET_URL)


def random_string(length: int) -> str:
    return "".join(random.choice(string.ascii_uppercase + string.digits) for i in range(length))


def validate_write_operation(func):
    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        password = random_string(5)
        click.echo("Attempting to validate write operation: " + func.__name__)
        click.echo("Please enter " + password + " to confirm")
        if click.prompt("Enter the above string") == password:
            return func(*args, **kwargs)
        else:
            click.echo("Invalid confirmation")
        return func

    return wrapper


@click.group()
def cli():
    pass


@click.command()
@click.option("--username", prompt=True)
@click.option("--password", prompt=True, hide_input=True)
def login(username, password):
    click.echo(f"{TARGET_URL}/login")
    response = client.post(
        f"/login", data={"uname": username, "password": password}, follow_redirects=True
    )
    if response.status_code == 200:
        click.echo("Login successful")
        keyring.set_password(TARGET_URL, "USERNAME", username)
        keyring.set_password(TARGET_URL, username, response.cookies["sid"])
        return True
    else:
        click.echo("Login failed")
        return False


@click.command()
def logout():
    username = keyring.get_password(TARGET_URL, "USERNAME")
    keyring.delete_password(TARGET_URL, username)
    keyring.delete_password(TARGET_URL, "USERNAME")
    client.cookies.clear()
    click.echo("Logged out")


@click.command()
@click.option("--message", prompt=True)
@validate_write_operation
def make_announcement(message):
    response = client.post(f"/api/v1/announcement", data={"message": message})
    if response.status_code == 200:
        click.echo("Announcement made")
    else:
        click.echo("Failed to make announcement: " + response.reason_phrase)


@click.command()
def clear_keyring():
    os.remove(keyring.get_keyring().file_path)


cli.add_command(login)
cli.add_command(logout)
cli.add_command(make_announcement)
cli.add_command(clear_keyring)


def noop() -> bool:
    username = keyring.get_password(TARGET_URL, "USERNAME")
    cookie = keyring.get_password(TARGET_URL, username) if username else None
    if cookie:
        client.cookies.set("sid", cookie)
    response = client.get(f"/judge/noop")
    if response.status_code == 200:
        return True
    else:
        click.echo("Not authenticated")
        return False


def getpass() -> str:
    return click.prompt("Enter keyring password", hide_input=True)


def init_keyring():
    kr = CryptFileKeyring()
    kr.keyring_key = getpass()
    keyring.set_keyring(kr)


def init():
    click.echo(f"Using target URL: {TARGET_URL}")
    init_keyring()
    if not noop() and not login():
        sys.exit(1)


if __name__ == "__main__":
    init()
    cli()
