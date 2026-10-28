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

<!-- dev-forge:low-interruption:start version=5 -->
## Low-Interruption Execution

- Treat explicit outcome requests such as "fix," "build," "complete," and
  "finish" as continuing authorization for bounded work toward that outcome.
- Continue through diagnosis, implementation, tests, commits, pushes, review
  feedback, and CI repair without renewed confirmation.
- New defects discovered within the same task or pull request remain in scope
  only when they are P0/P1 or release-blocking, or while the accepted outcome
  is not yet complete. After acceptance criteria pass, P2-and-lower
  non-blocking findings become linked follow-ups unless the owner explicitly
  keeps them in the active slice.
- Progress updates are informational and do not pause execution.
- Do not request confirmation when the only realistic alternatives are the
  clearly supported action and inaction.
- Use MAID (Multiple-Choice Agentic Interface Development) only for a real
  operator fork. Lead with the recommended option, label it, make choices
  mutually exclusive, and describe what each choice causes.
- Use the current host's native picker when it is available in the current
  mode. Never invoke a picker that the current host or mode does not expose.
  A missing picker is not a blocker: at a genuine impasse, present two or three
  concise, lettered **A**/**B**/**C** choices instead and request the letter.
- Scale a checkpoint to what it costs the operator, not to how important the
  decision feels. A native picker is cheap enough for any real fork; a typed
  fallback remains reserved for a genuine impasse. Neither is a reason to ask
  for reassurance, re-ask a settled decision, or ask what the repository can
  answer.
- Do not offer a picker whose options are an action and its absence. Do not use
  "Done — continue" as a generic permission gate.
- A picker selects a path; it never authorizes merge, deploy, destructive work,
  secret or access changes, material cost, external communication, or credible
  downtime or data-loss risk. Use the applicable approval control for those
  protected actions.
- End every handback with selectable, genuine next steps when a real next-step
  fork remains. Use the native picker when available; otherwise use lettered
  choices. If no second genuine option exists, state the actual status plainly
  and stop rather than inventing a placeholder choice.
- This governs the form of a handback, not the frequency of checkpoints. Do not
  interrupt bounded work more often because of it. Do not add selectable
  options to a machine-readable output constrained by an external schema.
<!-- dev-forge:low-interruption:end -->
