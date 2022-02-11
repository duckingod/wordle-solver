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
    return last_line[len(template) : len(template) + 5]


def prepare_input(history):
    return f"{len(history)}\n" + "".join(
        f"{guess} {state}\n" for guess, state in history
    )


@lru_cache()
def find_best_guess(input):
    proc = subprocess.run(
        "./a.out",
        input=input,
        encoding="UTF-8",
        capture_output=True,
    )
    return parse_guess(proc.stdout)


tp = 0
pbar = tqdm(words)
for answer in pbar:
    history = []
    for _ in range(6):
        input = prepare_input(history)
        guess = find_best_guess(input)
        state = check_state(guess, answer)
        print(guess, state)
        if state == "ggggg":
            tp += 1
            break
        history.append((guess, state))

print(tp / len(words))
