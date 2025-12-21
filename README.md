# envx

A environment variable manager written in C.

## Build
```bash
make
```

## Usage
```bash
envx <subcommand> <env> <command> [args...]
```

**Subcommands:**
- `exec` - Load environment and execute command
- `link` - Create `.env` symlink to `.env.<env>`

**Environment:** Name matches `.env.<env>` file (e.g., `dev` → `.env.dev`)

## Examples
```bash
# Run command with .env.dev
envx exec dev npm start

# Run command with .env.prod
envx exec prod python manage.py runserver

# Create .env → .env.prod symlink
envx link prod
```

## Setup

Create `.env.<name>` files in your project:
```bash
# .env.dev
DATABASE_URL=postgres://localhost/dev
API_KEY=dev-key-123

# .env.prod
DATABASE_URL=postgres://prod-server/db
API_KEY=prod-key-456
```

## Install
```bash
sudo make install

make clean
```

## Testing
```bash
# Check loaded variables
envx exec dev env | grep DATABASE

# Run any command
envx exec staging ./deploy.sh

# Use with shell
envx exec dev sh -c 'echo $DATABASE_URL'
```
