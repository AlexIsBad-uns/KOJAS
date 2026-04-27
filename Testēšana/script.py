import os, sys, time, threading, datetime
import urllib.request, urllib.parse

try:
    import serial
except ImportError:
    print("install pyserial first: python -m pip install pyserial")
    sys.exit(1)

IP = os.environ.get("KOJAS_IP", "192.168.4.1")
PORT = os.environ.get("KOJAS_PORT", "COM3")
WIFI = os.environ.get("KOJAS_WIFI", "Edgars iPhone")
PASS = os.environ.get("KOJAS_PASS", "12344321")
CAL = os.environ.get("KOJAS_CAL_API", "https://script.google.com/macros/s/AKfycbzBCagtL5N3NzHvaInUmQ6t9Yd-zhfCBbqvv8Ig2Z2LN7eepwoYzO41rAXBX2TlH8Lx/exec")
S1 = os.environ.get("KOJAS_S1_ID", "0611")
S2 = os.environ.get("KOJAS_S2_ID", "3119")


def check(cond, msg):
    print(("  PASS  " if cond else "  FAIL  ") + msg)


def save(extra):
    fields = {
        "ssid": WIFI, "pass": PASS,
        "lat": "56.953074", "lon": "24.081515", "utc": "3",
        "t_freq": "2", "w_ref": "30",
        "s1_id": "", "s1_nm": "Stop 1", "s1_del": "0", "s1_flt": "",
        "s2_id": "", "s2_nm": "Stop 2", "s2_del": "0", "s2_flt": "",
        "bdays": "", "alarms": "", "cal_api": "",
        "timey_api": "https://thetimey.com/api/public/rt/a2d8f81a31a3e33b3314cc2809fc8037?stopId=",
    }
    fields.update(extra)

    try:
        data = urllib.parse.urlencode(fields).encode()
        req = urllib.request.Request(f"http://{IP}/save", data=data, method="POST")
        req.add_header("Content-Type", "application/x-www-form-urlencoded")
        urllib.request.urlopen(req, timeout=10)
        print("  PASS  settings saved")
        return True
    except Exception as e:
        print(f"  FAIL  could not save settings: {e}")
        return False


def read_serial(seconds):
    lines = []
    stop = threading.Event()

    def reader():
        try:
            with serial.Serial(PORT, 115200, timeout=0.5) as s:
                while not stop.is_set():
                    line = s.readline().decode("utf-8", errors="replace").strip()
                    if line:
                        lines.append(line)
        except Exception as e:
            print(f"  FAIL  serial error: {e}")

    t = threading.Thread(target=reader, daemon=True)
    t.start()
    time.sleep(seconds)
    stop.set()
    t.join(2)
    return lines


def test6():
    print("\nTest 6 - birthday list sorted by proximity")

    bdays = "Valts (27.04.)\nJanis (24.06.)\nMarta (27.07.)\nAnna (26.07.)\nPeteris (29.06.)"
    if not save({"bdays": bdays}):
        return

    lines = read_serial(60)
    evts = [l for l in lines if l.startswith("BDAY:EVT,")]

    check("BDAY:BEGIN" in lines, "BDAY:BEGIN received")
    check("BDAY:END" in lines, "BDAY:END received")
    check(len(evts) > 0, f"got {len(evts)} birthday events")
    check(len(evts) <= 5, f"max 5 events ({len(evts)} received)")

    def days_until(line):
        d = line.split(",")[2].strip()
        day, month = int(d[:2]), int(d[3:5])
        today = datetime.date.today()
        bday = datetime.date(today.year, month, day)
        if bday < today:
            bday = bday.replace(year=today.year + 1)
        return (bday - today).days

    if len(evts) >= 2:
        days = [days_until(e) for e in evts]
        check(days == sorted(days), f"sorted by proximity: {days}")


def test7():
    print("\nTest 7 - empty birthday list")

    if not save({"bdays": ""}):
        return

    time.sleep(10)
    lines = read_serial(60)
    evts = [l for l in lines if l.startswith("BDAY:EVT,")]

    check(len(evts) == 0, f"no birthday events (got {len(evts)})")


def test8():
    print("\nTest 8 - google calendar")

    if not save({"cal_api": CAL}):
        return

    lines = read_serial(90)
    evts = [l for l in lines if l.startswith("CAL:EVT,")]

    check("CAL:BEGIN" in lines, "CAL:BEGIN received")
    check("CAL:END" in lines, "CAL:END received")
    check(len(evts) > 0, f"got {len(evts)} calendar events")
    check(len(evts) <= 5, f"max 5 events ({len(evts)} received)")

    for e in evts:
        title = e.split(",")[2].strip() if len(e.split(",")) >= 3 else ""
        check(len(title) <= 24, f"title length ok: '{title}'")


def test9():
    print("\nTest 9 - transit stops with 40 min walk offset")

    if not save({
        "s1_id": S1, "s1_nm": "Kipsala", "s1_del": "40", "s1_flt": "",
        "s2_id": S2, "s2_nm": "Pernavas", "s2_del": "40", "s2_flt": "",
    }):
        return

    lines = read_serial(90)

    check("CMD:BEGIN,0" in lines, "stop 1 data received")
    check("CMD:BEGIN,1" in lines, "stop 2 data received")

    for line in lines:
        parts = line.split(",")
        if len(parts) == 3 and parts[0] in ("bus", "tram", "trol"):
            try:
                secs = int(parts[2])
                check(secs >= 0, f"departure not negative ({secs}s)")
                check(secs <= 3600, f"departure within 1 hour ({secs}s)")
                check(True, f"valid type: {parts[0]}")
            except ValueError:
                pass


if __name__ == "__main__":
    try:
        urllib.request.urlopen(f"http://{IP}/", timeout=4)
    except Exception:
        print("device not reachable - connect to KOJAS Wi-Fi first")
        sys.exit(1)

    # Run only ONE test at a time:
    # test6()
    # test7()
    # test8()
    test9()

    print("\ndone")