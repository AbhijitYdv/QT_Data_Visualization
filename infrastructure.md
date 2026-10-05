# Infrastructure Notes

Team AWS instance for course work: `Fall2026_ToporskiSE_Team1`.

Connection details (IP address, SSH key, login steps) are kept in the team's private notes and are intentionally not stored in this repo.

## Specs and limits

| Field | Value |
|---|---|
| OS | Ubuntu LTS |
| Disk | ~8.6 GB root volume |
| RAM | ~1 GB |
| Open ports | 22 (SSH), 80 (HTTP), 443 (HTTPS) |

Other ports must be requested from the AWS admin (instance name, IP, port, reason).

## Intended use

- Serve static content: exported interactive HTML dashboards and a demo page.
- Deployment target only. The Qt application is **not** built on this server (1 GB RAM is too small); builds run in GitHub Actions.
- Not suited to heavy data processing or multiple containers without a swap file.

## GitHub connection

- The server pulls from this repo using a **read-only deploy key** (one repo only).
- Workflow: write code locally, push to GitHub, then `git pull` on the server.
- Teammates push from their own laptops using their own GitHub accounts.

## Rules and reminders

- Use only for authorized course work under the university's Acceptable Use Policy.
- **The instance is deleted at the end of the semester.** Back up exports and demo content before the December presentation.
- The instance can break and may need replacing. Keep setup steps scripted so a rebuild is quick.
- Tell teammates before rebooting, since it disconnects everyone.
- Never commit the private key (`.pem`) to GitHub or share it outside the team.

## Support contacts

- **AWS admin:** availability, restarts, replacement, opening ports. Only one designated team member contacts them.
- **Course instructor:** software installs, app configuration, code, databases, assignments.

Designated AWS contact: _TBD_
