# Contributing to PIFKID 2026

Thank you for your interest in contributing. This document covers the process for contributing to the PIFKID Smart Desk Clock project.

---

## Architecture Frozen Policy

**The architecture is FROZEN as of MVP v1.0.**

During the hardware bring-up phase, only the following changes are permitted:

1. Compilation errors
2. Boot failures
3. Missing REST endpoints required by the existing Web UI
4. Hardware initialization failures
5. TFT configuration issues
6. LittleFS/NVS failures
7. Crashes, watchdog resets, memory corruption
8. Verified logic bugs preventing MVP functionality

**NOT permitted:**

- New features
- Refactoring
- Code optimization
- Architecture changes
- New abstractions
- Module boundary changes
- New dependencies
- "Better" implementations

---

## Branch Naming

| Type | Format | Example |
|------|--------|---------|
| Feature | `feature/<short-desc>` | `feature/alarm-snooze` |
| Bug fix | `fix/<short-desc>` | `fix/alarm-double-trigger` |
| Hotfix | `hotfix/<short-desc>` | `hotfix/boot-crash` |
| Documentation | `docs/<short-desc>` | `docs/api-reference` |

---

## Commit Message Format

Use Conventional Commits:

```
<type>: <description>

[optional body]
```

### Types

| Type | When to use |
|------|-------------|
| `feat` | New feature (only during unfrozen phase) |
| `fix` | Bug fix |
| `docs` | Documentation only |
| `style` | Formatting, no code change |
| `refactor` | Code restructuring (only during unfrozen phase) |
| `test` | Adding tests |
| `chore` | Build, CI, tooling |

### Examples

```
fix: resolve alarm double-trigger on EventBus
docs: update API reference for display endpoint
test: add schedule CRUD persistence tests
```

---

## Pull Request Checklist

Before submitting a PR:

- [ ] Code compiles without warnings
- [ ] Simulation tests pass (`pio run -e simulation`)
- [ ] No new features (if architecture is frozen)
- [ ] No architecture changes (if architecture is frozen)
- [ ] Documentation updated (if applicable)
- [ ] Commit messages follow convention
- [ ] Branch is up to date with `main`

---

## Coding Rules

### File Naming
- Headers: `snake_case.h`
- Source: `snake_case.cpp`
- Constants: `UPPER_SNAKE_CASE`

### Class Naming
- PascalCase: `TodoRepository`, `AlarmService`
- Global instances: `todoRepo`, `alarmService`, `eventBus`

### Include Order
1. Own header
2. Project headers
3. Library headers

### Memory
- Use `char[]` arrays, not `Arduino String` in models
- Use `DynamicJsonDocument` with defined sizes
- Return `std::vector` by value (copy), not reference

### Thread Safety
- All repositories protected by `SemaphoreHandle_t _mutex`
- EventBus has separate mutexes for queue and subscriptions
- Never call repository methods from ISR context
- Never hold two mutexes simultaneously

### EventBus
- Emit events, don't call services directly
- Subscribe in `begin()` or `init()`
- Use string messages for sub-commands

---

## Development Workflow

1. Fork the repository
2. Create a feature branch from `main`
3. Make your changes
4. Run simulation tests
5. Submit a pull request
6. Wait for review and CI checks

---

## Questions?

Open an issue or refer to [AGENTS.md](AGENTS.md) for detailed project information.
