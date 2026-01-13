Event Thread Management System

## Overview

This project is a simple multithreaded **simulation** of an event management system that coordinates limited shared resources using POSIX threads, mutexes, and semaphores. It models multiple events competing for proposal spots, decor kits, and AV kits while enforcing a maximum number of concurrent events.

## Features

- Simulates a fixed number of events (`NUM_EVENTS`, default 8) each with random requirements and durations.[1]
- Limits global concurrency using a semaphore so that at most `MAX_CONCURRENT_EVENTS` events run at the same time (default 3).[1]
- Controls access to shared resources:
  - Proposal spots (`PROPOSAL_SPOTS`, default 3).
  - Decor kits (`DECOR_KITS`, default 3).
  - AV kits (`AV_KITS`, default 4).
- Uses a mutex-protected logger to print:
  - Waiting status and required resources for each event.
  - Execution start and completion, including current active event count.

## How it works

- Each event is represented by an `event_t` struct with:
  - `id`, `planner_id`, `need_proposal`, `need_decor`, `need_av`, and `duration` fields.
- `main()`:
  - Seeds the random generator and prints system capacity information.
  - Randomly initializes each event’s planner, resource needs, and duration (1–2 seconds).
  - Spawns one thread per event, staggering creation with a short `usleep` for realism.
  - Waits for all event threads to finish, then cleans up semaphores and mutex.
- Each event thread:
  - Logs its waiting state and desired resources.
  - Waits on the global concurrency semaphore, then increments `active_events` under a mutex.
  - Acquires proposal, decor, and AV resources using semaphores if required.
  - Runs (sleeps) for `duration` seconds, then releases all resources and decrements `active_events`.

## Build instructions

Ensure you have a POSIX-compliant environment (e.g., Linux) with pthreads and POSIX semaphores available or can use online compiler.


The program will:

- Print system capacity and resource configuration (max concurrent events and counts of each kit type).[1]
- Generate and display a list of events with randomly assigned planners, resource needs, and durations.[1]
- Start the simulation, logging:
  - When each event is waiting, acquires resources, starts running, and completes.[1]
  - Current active events and resource acquisition/release for proposal spots, decor kits, and AV kits.[1]
