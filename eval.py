import json
import subprocess
from functools import lru_cache

from tqdm import tqdm

words = json.load(open("dictionary.json"))
template = "Best guess: "


def check_state(guess, answer):
    result = []
    for g, a in zip(guess, answer):
        if g == a:
            result.append("g")
        elif g in answer:
            result.append("Y")
        else:
            result.append("-")
    return "".join(result)


def parse_guess(out: str):
    last_line = out.splitlines()[-1]
    return last_line.strip()


def prepare_input(history):
    return ",".join(f"{guess},{state}" for guess, state in history)


@lru_cache()
def find_best_guess(input):
    proc = subprocess.run(
        ["./a.out", input],
        encoding="UTF-8",
        capture_output=True,
    )
    return parse_guess(proc.stdout)


def start_game(answer):
    history = []
    for _ in range(6):
        input = prepare_input(history)
        guess = find_best_guess(input)
        state = check_state(guess, answer)
        pbar.write(f"{guess} {state}")
        if state == "ggggg":
            return True
        history.append((guess, state))


tp = 0
pbar = tqdm(words)
for answer in pbar:
    is_win = start_game(answer)
    if is_win:
        tp += 1

print(tp / len(words))
