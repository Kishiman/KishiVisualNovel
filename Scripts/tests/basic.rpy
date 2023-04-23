# This is a comment

# Variable assignment
$ variable_name = "Hello, world!"
$ number_variable = 10

# If statement
if number_variable > 5:
    $ variable_name = "Number variable is greater than 5!"

# While loop
$ loop_variable = 0
while loop_variable < 10:
    $ loop_variable += 1
    "Loop variable is now $loop_variable"

# Function definition
define my_function():
    "This is a function."

# Function call
my_function()

# Display text
label start:
    "This is some text."
    "This is more text."
    "This is text on a new line.\nThis is more text on the same line."

# Display images
image bg "background.png"
show character "character.png"

# Play music
play music "music.mp3"

# Play sound effects
play sound "sound.wav"

# Screen transitions
with dissolve:
    "This text will dissolve in and out."

# Menu
menu:
    "Choose an option."
    "Option 1":
        "You chose option 1."
    "Option 2":
        "You chose option 2."

# End of script
return
