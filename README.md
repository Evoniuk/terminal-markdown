# Terminal Markdown

## What is it?

Terminal markdown is a simple formatting tool for displayng text on a UNIX-like terminal emulator. It allows you to easily create text that is

- Bold
- Dim
- Italic
- Underlined
- Blinking
- Inverted
- Hidden
- Striked through
- Red
- Green

and render it on a terminal emulator.

Note that some of the above styles are not supported everywhere. Particulary italic, blinking, and hidden.

## How do I try it?

To get started, type the following into your terminal:

```zsh
% git clone https://github.com/Evoniuk/terminal-markdown.git
% cd terminal-markdown
% make tmd && ./tmd intro.tmd
```

If everything works right, this will display a brief tutorial on how to use terminal markdown.

### Instalation

To install `tmd` for global use, run the following from the `terminal-markdown` directory:

**For zsh:**
```zsh
% echo 'function tmd() {`pwd`/tmd $*}' >> ~/.zshrc
```

**For bash:**
```bash
% echo 'function tmd() {`pwd`/tmd $*}' >> ~/.bashrc
```


## How do I use it?

Terminal markdown works much like markdown. Text is styled by wrapping it in special characters.

The association of characters to styles is

```
# Bold
~ Dim
* Italic
_ Underline
@ Blinking
$ Inverted
` Hidden
% Strikethrough
^ Red
| Green
```

So, for example, if you put the following in a file named `hello.tmd`

```
#Hello# *World*!
```

and run `tmd hello.tmd`, you'll get the following displayed on the terminal:

> **Hello** *World*!
