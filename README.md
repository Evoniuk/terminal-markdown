# Terminal Markdown

## What is it?

Terminal markdown is a simple formatting tool for text displayed on a UNIX-like terminal. It allows you to easily create text that is

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

when displayed on a terminal.

Note that some of the above styles are not supported everywhere. Particulary italic, blinking, hidden, and strikethrough.

## How do I try it?

To get started, type the following into the terminal:

```zsh
% git clone https://github.com/evoniuk/terminal-markdown.git
% cd terminal-markdown
% make tmd && ./tmd intro.tmd
```

If everything works right, this will display a brief tutorial on how to use terminal markdown.

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
