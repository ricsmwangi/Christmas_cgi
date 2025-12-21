# Git & GitHub Best Practices for Portfolio Projects

## 📋 Overview

Professional project management strategy for showcasing your systems programming work on GitHub.

---

## 🎯 Commit Strategy

### Commit Frequency

**Do:** Commit after each working feature
```bash
git add project_name/
git commit -m "Feat: add project_name - syscalls: fork, pipe, signal"
git push origin main
```

**Don't:** Wait until project is 100% finished to commit
```bash
# Bad: One giant commit with everything
git add .
git commit -m "Week50 complete"
```

### Commit Message Format

```
<type>: <subject>

<body>

<footer>
```

**Types:**
- `feat:` - New feature or project
- `fix:` - Bug fix
- `docs:` - Documentation update
- `style:` - Code formatting
- `refactor:` - Restructure code
- `test:` - Add/update tests
- `chore:` - Build system, dependencies

**Subject line:**
- Imperative mood ("add", not "added" or "adds")
- No period at end
- Under 50 characters
- Specific (not just "update code")

**Examples:**

```bash
# Good
git commit -m "Feat: add mycp with progress bar - syscalls: open, read, write, stat"

# Good
git commit -m "Fix: handle EOF correctly in simple_shell - waitpid status macros"

# Good
git commit -m "Docs: add FUNDAMENTALS.md with syscall explanations"

# Bad
git commit -m "updated stuff"

# Bad
git commit -m "Week50"

# Bad
git commit -m "Feat: add mycp with progress bar and error handling and multiple buffer sizes"
```

---

## 📦 Project Structure for Portfolio

```
week50/
├── SYSTEMS_PROGRAMMING_WEEK50.md  ← Overview for recruiters
├── FUNDAMENTALS.md                 ← Study guide (worth seeing)
├── GIT_PUSH_GUIDE.md              ← This file
│
├── 01_file_io/
│   ├── README.md                  ← Explains project + learning
│   ├── mycp.c                     ← Well-commented source
│   ├── Makefile                   ← Professional build
│   └── NOTES.txt                  ← What you learned
│
├── 02_process_management/
│   ├── README.md
│   ├── simple_shell.c
│   ├── Makefile
│   └── NOTES.txt
│
└── 03_ipc_signals/
    ├── README.md
    ├── producer.c
    ├── consumer.c
    ├── Makefile
    └── NOTES.txt
```

**What recruiters see:**
1. README.md → Project purpose and how to build/run
2. Source code → Code quality, comments, error handling
3. Makefile → Professional build system
4. NOTES.txt → What you learned (growth mindset!)

---

## 📝 README Template for Each Project

```markdown
# Project Name

## What This Project Does

[2-3 sentence summary]

## Learning Objectives

- ✅ Understand concept 1
- ✅ Understand concept 2
- ✅ Practice syscall X, Y, Z

## Build & Run

\`\`\`bash
make
./program [args]
\`\`\`

## Key Concepts

- **Syscall 1**: Explanation
- **Syscall 2**: Explanation

## Testing

\`\`\`bash
make test
\`\`\`

## Challenges & Solutions

Describe any problems you solved.

## Extensions

Future improvements.
```

---

## 🚀 Recommended Workflow

### For Each Project

```bash
# Day 1: Build basic version
vim project_name.c
make
./test.sh
git add project_name/
git commit -m "Feat: add project_name basic functionality"
git push origin main

# Day 2: Add error handling
vim project_name.c
make
git add project_name/
git commit -m "Fix: comprehensive error handling in project_name"
git push origin main

# Day 3: Add extensions
vim project_name.c
make
git add project_name/
git commit -m "Feat: add progress bar to project_name"
git push origin main

# Day 4: Documentation
vim README.md
git add project_name/README.md
git commit -m "Docs: add README for project_name with examples"
git push origin main
```

### Weekly Summary Commit

```bash
# Friday after completing all projects
git add week50/
git commit -m "Week50: complete systems programming (5 projects, 50+ syscalls)"
git push origin main
```

---

## 💾 Push Strategy

### After each working session:

```bash
# Add changes
git add .

# Review before committing
git status

# Commit with meaningful message
git commit -m "Feat: add feature - explains what was added"

# Push to GitHub
git push origin main
```

### Never push:

- ❌ Binary files (`.o`, `.out`, executables)
- ❌ Temporary files (`*~`, `*.swp`)
- ❌ Build artifacts
- ❌ `.gitignore` already handles these

---

## 🔄 Example Week50 Commits

```
Dec 6 - Initial setup
  commit: "Init: scaffold week50 directory structure"

Dec 7 - Project 1
  commits:
    "Feat: add mycp with basic copy functionality"
    "Feat: add progress bar to mycp"
    "Fix: handle large files in mycp"
    "Docs: add README for mycp project"

Dec 8 - Project 2
  commits:
    "Feat: add simple_shell with fork/exec"
    "Feat: add built-in commands (cd, pwd)"
    "Feat: add background job support"
    "Docs: add README for simple_shell"

...and so on...

Dec 10 - Integration
  commits:
    "Feat: add file_server daemon integrating all concepts"
    "Docs: update WEEK50 overview"
    "Chore: finalize .gitignore for week50"
    "Week50: complete systems programming masterclass"
```

When recruiters look at your GitHub:
- They see consistent, regular commits (shows real work)
- Each commit is focused (shows understanding)
- Commit messages are clear (shows communication)
- Clean progression (shows planning)

---

## 📊 GitHub Profile Optimization

### Profile README

```markdown
# Hi, I'm [Your Name]

## Currently Learning

- Systems Programming (Unix/Linux syscalls)
- C language systems projects
- Open-source contributions

## Featured Projects

- [learning-progress](link) - C programming journey
- [graduation-gift](link) - Web development (graduation site)

## Tech Stack

- **Languages**: C, JavaScript, Python
- **Skills**: Systems programming, Unix APIs, Git
```

### Project Showcase

Pin top 3 projects to profile:
1. week49 (student_management) - shows structs + memory mastery
2. week50 (systems_programming) - shows Unix syscalls
3. graduation-gift - shows full-stack capability

### Contribution Activity

- Consistent commits show dedication
- Public learning repos show transparency
- Multiple projects show versatility

---

## 🔐 Security Reminders

### What NOT to commit:

```bash
# API keys, passwords
API_KEY = "sk_live_abc123..."

# SSH keys
~/.ssh/id_rsa

# Token files
.env with credentials

# Database files
*.db, *.sqlite
```

### Use .gitignore:

```bash
# Already done in learning-progress, but verify:
*.key
*.pem
.env
*.db
/tmp/
```

---

## 🎯 Before Pushing: Checklist

- [ ] Code compiles: `make`
- [ ] No warnings: `-Wall -Wextra -pedantic`
- [ ] Program runs: `./program`
- [ ] Tests pass: `make test`
- [ ] README is up to date
- [ ] No binary files added
- [ ] Commit message is clear
- [ ] Related files grouped in one commit

---

## 📚 Viewing Your Progress on GitHub

```bash
# View commit history
git log --oneline -20

# View specific commit
git show <commit-hash>

# Compare branches
git diff main develop

# View commits in this week
git log --since="1 week ago" --oneline
```

---

## 🚀 After Week50: Open Source Contributions

Once projects pushed to GitHub:

1. **Showcase on profile** - link to week50
2. **Search for open issues** - "good-first-issue" repos
3. **Fork target repo** - make changes on feature branch
4. **Push to your fork** - your public contributions
5. **Open PR** - with clear description

Your systems programming projects make great portfolio for:
- Linux kernel contributions
- System utility projects
- Infrastructure tools
- Networking libraries

---

## 📝 Example PR Description (for later)

```markdown
## Description

This PR fixes issue #1234 by improving error handling in the 
file descriptor management code.

## Changes

- Added proper error checking for open() syscalls
- Added cleanup on failure paths
- Updated error messages

## Testing

- Tested with 100MB files
- Tested error cases (permission denied, file not found)
- No regressions in existing tests

## Why This Matters

The original code would leak file descriptors on errors, 
which I learned about in systems programming study.

Related issue: #1234
```

---

## 🎓 Learning Outcomes

By following this workflow, you demonstrate:

- ✅ **Professionalism:** Clean commits, good messages
- ✅ **Consistency:** Regular pushes show dedication
- ✅ **Communication:** Clear READMEs and documentation
- ✅ **Progression:** Git history shows growth
- ✅ **Collaboration:** Ready for open-source work

---

**Ready to push Week50 to GitHub? You got this! 🚀**
