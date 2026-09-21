"""Validate (or compile) the shipped YAML against this checkout, with fake secrets.

Run using the Python environment containing ESPHome: python tests/check_firmware.py
Add --compile for a full ESP32-S3 build. No device is flashed.
"""
import base64
from pathlib import Path
import subprocess
import sys

import yaml

ROOT = Path(__file__).resolve().parents[1]


class Loader(yaml.SafeLoader):
    pass


SECRETS = {
    "wifi_ssid": "firmware-validation-only",
    "wifi_password": "firmware-validation-only",
    "spa_api_key": base64.b64encode(bytes(range(32))).decode(),
}
Loader.add_constructor("!secret", lambda loader, node: SECRETS[loader.construct_scalar(node)])
config = yaml.load((ROOT / "zavepower_energy_optimizer.yaml").read_text(), Loader=Loader)
config["external_components"] = [{
    "source": {"type": "local", "path": str(ROOT / "components")},
    "components": ["balboa_spa"],
}]
config["esphome"]["build_path"] = str(ROOT / ".build" / "firmware")
output = ROOT / ".build" / "validation.yaml"
output.parent.mkdir(exist_ok=True)
output.write_text(yaml.safe_dump(config, sort_keys=False))
command = "compile" if "--compile" in sys.argv else "config"
subprocess.run([sys.executable, "-m", "esphome", command, str(output)], check=True)
