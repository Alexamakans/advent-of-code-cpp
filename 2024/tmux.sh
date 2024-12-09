#!/usr/bin/env bash

SCRIPT_ROOT=$(pwd)
INPUT_DIR=$SCRIPT_ROOT/inputs
SOURCE_DIR=$SCRIPT_ROOT/src

DAY=$1

SESSION_NAME="AdventOfCode2024"
if [[ $# -eq 2 ]]; then
  SESSION_NAME="AdventOfCode2024_Test"
fi
CMD_NVIM="nvim $SCRIPT_ROOT/src/${DAY}.cpp"
CMD_WATCH_TEST="$SCRIPT_ROOT/watch.sh $1 test"
CMD_WATCH="$SCRIPT_ROOT/watch.sh $1"

tmux has-session -t "$SESSION_NAME" 2> /dev/null
if [[ $? != 0 ]]; then
  tmux new-session -d -s "$SESSION_NAME"

  if [[ $# -eq 2 ]]; then
    tmux split-window -h -t "$SESSION_NAME"
    tmux send-keys -t "$SESSION_NAME:0.1" "$CMD_WATCH_TEST" C-m
  else
    tmux split-window -h -t "$SESSION_NAME"
    tmux send-keys -t "$SESSION_NAME:0.1" "$CMD_WATCH" C-m
  fi

  tmux select-pane -t "$SESSION_NAME:0.0"
  tmux send-keys -t "$SESSION_NAME:0.0" "$CMD_NVIM" C-m
  tmux resize-pane -t "$SESSION_NAME:0.0" -R 20
fi

tmux attach -t "$SESSION_NAME"
