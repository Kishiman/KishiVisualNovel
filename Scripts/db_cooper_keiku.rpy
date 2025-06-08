# Define Characters

define narrator = Character("")
define cooper = Character("Cooper", color="#FF0000")  # D.B. Cooper is the protagonist
define flight_attendant = Character("Flight Attendant", color="#00BFFF")
define pilot = Character("Pilot", color="#FFA500")

# Images
image bg_airplane_sitting = "DBCooperKeiku/bg_airplane_sitting1"
image bg_briefcase = "DBCooperKeiku/bg_briefcase1"
image db_cooper = "DBCooperKeiku/db_cooper"

# Sounds and Music
sound hard_heart = "EliotHolmes/Hard_Heart"
sound footsteps = "pixabay/footsteps-in-a-hallway"
sound tense_music = "Lemmino/Aloft_BGM"

# Scene 1: Onboarding at the Airport
label start:
    play music tense_music loop
    scene bg_airplane_sitting with fade
    play sound footsteps loop
    
    narrator "Before she walks away, you lean in slightly, keeping your voice low."
    
    stop sound
    play music hard_heart fadein 3 loop

    show db_cooper at left
    cooper "Miss, you might want to read that now."
    hide db_cooper

    narrator "Her smile falters as she opens the note. Her eyes widen in shock."

    scene bg_briefcase with fade



