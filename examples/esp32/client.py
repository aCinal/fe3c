import requests
import argparse
import logging

def main() -> None:
    """Application entry point."""
    opts = parse_args()
    log = init_logger(opts.verbose)

    hostname = f"http://{opts.ip}"

    if opts.action == "set_secret":
        if opts.key == None:
            log.error("set_secret requires a secret key file (-k/--key)")
            return

        log.debug(f"Opening '{opts.key}'...")
        with open(opts.key, "rb") as fd:
            log.debug("Setting the secret key...")
            set_secret_key(fd.read(), opts.curve, hostname)

    elif opts.action == "get_public":
        if opts.out == None:
            log.error("get_public requires an output file (-o/--out)")
            return

        log.debug(f"Opening '{opts.out}'...")
        with open(opts.out, "wb") as fd:
            log.debug("Getting the public key...")
            fd.write(get_public_key(opts.curve, hostname))

    elif opts.action == "sign":
        if opts.out == None:
            log.error("sign requires an output file (-o/--out)")
            return
        if opts.message == None:
            log.error("sign requires a message file (-m/--message)")
            return

        log.debug(f"Opening '{opts.message}' and '{opts.out}'...")
        with open(opts.out, "wb") as outfd, open(opts.message, "rb") as infd:
            log.debug("Both files opened successfully. Signing...")
            outfd.write(sign(infd.read(), opts.curve, hostname))

    elif opts.action == "verify":
        if opts.message == None:
            log.error("verify requires a message file (-m/--message)")
            return
        if opts.signature == None:
            log.error("verify requires a signature file (-s/--signature)")
            return

        log.debug(f"Opening '{opts.message}' and '{opts.signature}'...")
        with open(opts.signature, "rb") as sigfd, open(opts.message, "rb") as msgfd:
            log.debug("Both files opened successfully. Verifying...")
            print(str(verify(msgfd.read(), sigfd.read(), opts.curve, hostname), "ascii"))

def set_secret_key(key: bytes, curve: str, hostname: str) -> None:
    """Set a new secret key for a given curve."""
    resp = requests.put(hostname + f"/secret?curve={curve}", data=key)
    assert resp.status_code == 200

def get_public_key(curve: str, hostname: str) -> bytes:
    """Fetch the public key for a given curve."""
    resp = requests.get(hostname + f"/public?curve={curve}")
    assert resp.status_code == 200
    return resp.content

def sign(message: bytes, curve: str, hostname: str) -> bytes:
    """Sign a given message."""
    resp = requests.post(hostname + f"/sign?curve={curve}", data=message)
    assert resp.status_code == 200
    return resp.content

def verify(message: bytes, signature: bytes, curve: str, hostname: str) -> str:
    """Verify an existing signature against a message."""
    resp = requests.post(hostname + f"/verify?curve={curve}", data=signature + message)
    assert resp.status_code == 200
    return resp.content

def parse_args() -> argparse.Namespace:
    """Parse the command line arguments."""
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "action",
        choices=("sign", "verify", "set_secret", "get_public")
    )
    parser.add_argument(
        "-i",
        "--ip",
        help="Signing oracle's IP address",
        default="192.168.4.1"
    )
    parser.add_argument(
        "-v",
        "--verbose",
        help="Print debug logs",
        action="store_true",
    )
    parser.add_argument(
        "-c",
        "--curve",
        help="Edwards curve of choice",
        choices=("ed25519", "ed448"),
        default="ed25519"
    )
    parser.add_argument(
        "-s",
        "--signature",
        help="Signature file"
    )
    parser.add_argument(
        "-m",
        "--message",
        help="Message file"
    )
    parser.add_argument(
        "-k",
        "--key",
        help="Secret key file"
    )
    parser.add_argument(
        "-o",
        "--out",
        help="Output file"
    )
    return parser.parse_args()

def init_logger(verbose: bool):
    """Initialize the logger."""
    logging.basicConfig(
        format = "[%(levelname)s] %(asctime)s (%(threadName)s): %(message)s",
        level = logging.DEBUG if verbose else logging.INFO,
    )
    return logging.getLogger()

if __name__ == "__main__":
    main()
