# Lineote
_Track your thoughts like a boss. All in the terminal._

This is a simple terminal thought-manager quickly thrown together in C for fun.

# Disclaimer
This project is not really finished. It is works but it has limited functionality.

# Build
```bash
git clone https://github.com/ajTronic/lineote 
cd lineote
gcc main.c -O3
sudo mv ./a.out /usr/bin/lineote
```

# Usage
```bash
lineote # make a new entry
lineote all # view all past entries
lineote last # view last entry
lineote last 3 # view last 3 entries
```
Notes are stored in `~/.lineote_messages`
