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

It will render a file like this:

![before](before.png)

like this:

![after](after.png)

(In this screenshot you can't see the `BLINKING` text actually blink, and the `HIDDEN` text isn't hidden because of my particular terminal emulator.)

Note that some of the above styles are not supported everywhere. Particulary italic, blinking, and hidden.

## How do I try it?

To get started, type the following into your terminal:

```zsh
% git clone https://github.com/Evoniuk/terminal-markdown.git
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

You can display multiples files by calling `tmd file1 file2 ...`.

You can also pipe files like so:

```zsh
% cat intro.tmd | tmd
% curl https://raw.githubusercontent.com/Evoniuk/terminal-markdown/master/intro.tmd | tmd
```

## How does it work?

For the internals, all that's going on is a substitution of certain characters with certain escape sequences.

The first time the programs sees a special character, it substitutes it for an escape sequence according to the following:

```
Special Character       Escape Sequence         Style to Begin
-----------------       ---------------         --------------
#                       \e[1m                   bold
~                       \e[2m                   dim
*                       \e[3m                   italic
_                       \e[4m                   underline
@                       \e[5m                   blink
$                       \e[7m                   inverted
`                       \e[8m                   hidden
%                       \e[9m                   strikethrough
^                       \e[31m                  red
|                       \e[32m                  green
```

This escape sequence tells the terminal to start the associated style.

The next time the special character is seen, this substitution takes place:

```
Special Character       Escape Sequence         Style to End
-----------------       ---------------         ------------
#                       \e[22m                  bold
~                       \e[22m                  dim
*                       \e[23m                  italic
_                       \e[24m                  underline
@                       \e[25m                  blink
$                       \e[27m                  inverted
`                       \e[28m                  hidden
%                       \e[29m                  strikethrough
^                       \e[39m                  red
|                       \e[39m                  green
```

This escape sequence tells the terminal to end the associated style.
