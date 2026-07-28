# AGENTS.md

## Repo purpose

Dev-Forge is the standards repository for developer-oriented workflow, agent tooling, skill templates, and repo-shape governance in the OpenClaw ecosystem.

## Stack

- Markdown/docs-first repository with Bash scripts, Python tooling, and a small C#/.NET ops-dashboard subtree.
- GitHub CLI, Git workflows, and Docker-based local tool deployment surfaces are part of the repo ecosystem.

## Important paths

- `README.md` — canonical repo framing and owner-facing quick start.
- `repo-state.md` — current durable state mirror.
- `docs/` — conventions, workflows, and execution model.
- `templates/` — reusable templates used by downstream repos.
- `tools/` and `scripts/` — operational and repo-shaping automation.
- `.github/` — workflow and GitHub metadata.

## Common commands

- Restore/install: `unknown`
- Build: `unknown`
- Test: `unknown`
- Lint/format: `unknown`
- Run locally: `unknown`

## Validation expectations

- Run issue-scoped validation and report commands with outcomes.
- For instruction doc work, verify files exist and required headings are present.
- For automation or workflow work, run the smallest meaningful command set and capture failures/output.
- Report: files changed, validation run, risks, unresolved questions.

## Coding conventions

- Keep edits focused and reviewable.
- Prefer small, explicit, issue-scoped changes.
- Preserve existing patterns unless there is a strong reason to change them.
- Do not invent commands or assumptions about runtime behavior.

## Agent boundaries

- Do not edit secrets, production credentials, callback/auth URLs, or deployment infra without explicit approval.
- Avoid broad repository refactors from narrow issue tickets.
- Do not alter `.env` values or secret material.
- Do not invent commands or add command claims not confirmed by repo artifacts.
- Do not rewrite shared history unless explicitly asked.

## Context strategy

- Read `README.md`, `docs/`, and relevant issue body first.
- Prefer repo-local docs over guessing.
- If a stack question is unclear, inspect a representative sample file before broad edits.
- Keep one issue packet per action.

## Task packet expectations

- Return a concise summary with changed files, commands run, risks, unresolved questions, and next recommended task.

## Global DevOps GitHub–Kanban Contract

For DevOps, infrastructure, deployment, security, GitOps, and service work:

1. GitHub is authoritative for Issues, PRs, CI, reviews, merges, releases, and delivery state. Hermes Kanban is the execution queue only.
2. One GitHub Issue plus one active Hermes Kanban card should normally produce one PR directly to `main`.
3. Before branch work or a PR, fetch `origin/main` and reconcile against the current remote default branch. Do not build branch-on-branch PR stacks unless an explicit integration owner and final target are stated.
4. Do not merge, deploy, close issues, rotate secrets, or claim production success unless the task explicitly authorizes it and verification evidence exists.
5. If branches diverge, stop merging the stack. Create one integration branch from current `origin/main`, resolve semantic conflicts deliberately, run tests, and open one replacement PR to `main`.
6. For security or infrastructure work, provide exact build, test, and diff evidence and require fresh independent review before merge. Never put secrets in code, logs, PRs, or comments.
7. A task is not complete because a local test passes or a Kanban card says done. Completion requires the requested GitHub state and, when applicable, verified live behavior.
8. When creating a PR, state its target branch, linked issue, validation output, and whether it supersedes prior PRs. Do not leave divergent worker PRs ambiguous.

<!-- dev-forge:low-interruption:start version=1 -->
## Low-Interruption Execution

- Treat explicit outcome requests such as "fix," "build," "complete," and
  "finish" as continuing authorization for bounded work toward that outcome.
- Continue through diagnosis, implementation, tests, commits, pushes, review
  feedback, and CI repair without renewed confirmation.
- New defects discovered within the same task or pull request remain in scope
  when the repair is reversible, clearly supported, and consistent with the
  existing architecture.
- Progress updates are informational and do not pause execution.
- Do not request confirmation when the only realistic alternatives are the
  clearly supported action and inaction.
- Use a blocking checkpoint only at a genuine impasse. Present two or three
  materially different choices as **A**, **B**, and optionally **C**; recommend
  one and ask for a one-letter reply.
- Do not use "Done — continue" as a generic permission gate.
- Preserve explicit approval boundaries for merge, deploy, destructive work,
  secret or access changes, material cost, external communication, and credible
  downtime or data-loss risk.
<!-- dev-forge:low-interruption:end -->
