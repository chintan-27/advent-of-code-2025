# 🎄 Advent of Code 2025 — C++ Solutions

This repository contains my C++ solutions for **Advent of Code 2025**, along with a small tooling system that automates daily setup, compilation, and execution.

---

## Features

* Modern **C++20** solutions
* Automatic day initialization:

  * Creates `DayX/`
  * Adds `problem/` folder
  * Creates empty `input.txt`
  * Copies boilerplate solution
* Simple run command:

  ```bash
  ./init run DayX
  ```
* Helper utilities:

  * Logging (info/debug/error)
  * High-resolution timers
  * String trimming

---

## Project Structure

```
.
├── Day1/
│   ├── input.txt
│   ├── solution.cpp
│   └── problem/
│       ├── part1.txt
│       └── part2.txt
├── helpers/
│   └── helper.h
├── boilerplate.cpp
├── init
└── README.md
```

---

## Build & Run

Create a new day:

```bash
./init Day3
```

Compile + run:

```bash
./init run Day3
```

Manual compilation (optional):

```bash
g++ DayX/solution.cpp -std=c++20 -O2 -o DayX/output
./DayX/output
```

---

## Progress & Results

This format supports **any number of parts** per day.
<!-- RESULTS_START -->
| Day | Status  | Parts Completed | Time     | Stars earned           |
| --- | ------- | --------------- | -------- | --------------- |
| 1   | Done    | Part 1, Part 2  | ~0.40 ms | ⭐️⭐️ |
| 2   | Pending | —               | —        | —               |
<!-- RESULTS_END -->

<!-- ---

# 📘 Solution Gallery (Coming Soon ma)

*A text-based overview of each day’s puzzle and approach.*

You can expand this section as you solve more days.

--- -->

## Helper Library

`helpers/helper.h` provides:

* `Log::info()`, `Log::debug()`, `Log::error()`
* `Timer::start()` and `Timer::print()`
* `strip()` for trimming input lines

---

## License

MIT License.

---
