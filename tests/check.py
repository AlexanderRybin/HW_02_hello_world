import os
import sys
import subprocess
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parent.parent
MODULE_NAME = "hello_world"
MODULE_PATH = PROJECT_ROOT / "build" / f"{MODULE_NAME}.ko"


SYSFS_BASE = Path(f"/sys/module/{MODULE_NAME}/parameters")

CHECK_STR = "Hello, World!"

def run_cmd(cmd, check=True):
    result = subprocess.run(
        cmd,
        shell=True,
        capture_output=True,
        text=True,
    )
    if check and result.returncode != 0:
        raise RuntimeError(
            f"Команда '{cmd}' завершилась с ошибкой:\n"
            f"stdout: {result.stdout}\n"
            f"stderr: {result.stderr}"
        )
    return result


def is_module_loaded():
    result = run_cmd(f"lsmod | grep -w {MODULE_NAME}", check=False)
    return result.returncode == 0


def load_module():
    if is_module_loaded():
        unload_module()
    
    cmd = f"insmod {MODULE_PATH}"
    
    run_cmd(cmd)


def unload_module():
    """Выгрузить модуль."""
    if is_module_loaded():
        run_cmd(f"rmmod {MODULE_NAME}")


def read_param(param_name: str) -> str:
    """Прочитать текущее значение параметра из sysfs."""
    path = SYSFS_BASE / param_name
    with open(path, "r") as f:
        return f.read().strip()


def write_param(param_name: str, value) -> None:
    """Записать значение параметра через sysfs."""
    path = SYSFS_BASE / param_name
    with open(path, "w") as f:
        f.write(str(value))

if __name__ == "__main__":
    if os.geteuid() != 0:
        print("   Ошибка: скрипт должен быть запущен от root!")
        print("   Используйте: sudo python3 test_module.py")
        sys.exit(1)

    if is_module_loaded() == 0:
        load_module()

    for i, ch in enumerate(CHECK_STR):
        write_param("idx", i)
        write_param("ch_val", ord(ch))
        print(f"[{i}]  '{ch} = {ord(ch)}'")

    result_str = read_param("my_str")

    if result_str != CHECK_STR:
        print(f"Тестирование завершилось с ошибкой. Строка для записи: \"{CHECK_STR}\", строка в sysfs: \"{result_str}\"")
    else:
        print(f"Тестирование завершилось успешно. Строка для записи: \"{CHECK_STR}\", строка в sysfs: \"{result_str}\"")

    unload_module()