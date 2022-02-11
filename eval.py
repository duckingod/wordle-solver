import json
import subprocess
from functools import lru_cache
from sys import stdout

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


def start_game(agent, answer):
    guess = agent.send("next")
    for _ in range(6):
        state = check_state(guess, answer)
        if state == "ggggg":
            return True
        guess = agent.send(state)


def wordle_agent():
    p = subprocess.Popen(
        ["./a.out", "--interactive"],
        encoding="UTF-8",
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
    )
    while True:
        guess = p.stdout.readline().strip()
        state = yield guess
        pbar.write(f"{guess} {state}")
        print(state, file=p.stdin)
        p.stdin.flush()


tp = 0
pbar = tqdm(words)
agent = wordle_agent()
next(agent)
for answer in pbar:
    is_win = start_game(agent, answer)
    if is_win:
        tp += 1

print(tp / len(words))
