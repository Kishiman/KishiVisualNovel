# Define Characters

define narrator = Character("")
define cooper = Character("Cooper", color="#FF0000")  # D.B. Cooper is the protagonist
define flight_attendant = Character("Flight Attendant", color="#00BFFF")
define pilot = Character("Pilot", color="#FFA500")

default romance = False
# Images
image attendant_get_ransom = "DBCooperKeiku/attendant_get_ransom"
image attendant_go_cockpit = "DBCooperKeiku/attendant_go_cockpit"
image attendant_hand_parachute = "DBCooperKeiku/attendant_hand_parachute"
image attendant_offboard = "DBCooperKeiku/attendant_offboard"
image attendant_read_note = "DBCooperKeiku/attendant_read_note"
image cockpit_pilot = "DBCooperKeiku/cockpit_pilot"
image hand_note = "DBCooperKeiku/hand_note"
image note = "DBCooperKeiku/note"
image plane_land = "DBCooperKeiku/plane_land"
image plane_takeoff = "DBCooperKeiku/plane_takeoff"
image romance_start = "DBCooperKeiku/romance_start"
image romance_end = "DBCooperKeiku/romance_end"

image bg_briefcase = "DBCooperKeiku/bg_briefcase1"

# Sounds and Music
sound hard_heart = "EliotHolmes/Hard_Heart"
sound footsteps = "pixabay/footsteps-in-a-hallway"
sound tense_music = "Lemmino/Aloft_BGM"

# Scene 1: Onboarding at the Airport
label start:
    play music tense_music loop
    play sound footsteps loop
    scene hand_note with dissolve

    narrator "Before she walks away, you lean in slightly, keeping your voice low."
    cooper "Miss, I need you to do something for me. It's very important."
    flight_attendant "What is it, sir?"
    scene attendant_read_note with dissolve
    narrator "You hand her a note, your expression serious."
    narrator "Her smile falters as she opens the note. Her eyes widen in shock."
    
    stop sound
    play music hard_heart fadein 3 loop

    scene note with dissolve
    narrator "The note reads:"
    narrator "\"I have a bomb. I want $200,000 in cash, four parachutes, and a fuel truck standing by in Seattle. This is not a joke.\""
    
    scene bg_briefcase with dissolve
    narrator "You point to the briefcase."
    cooper "This is the briefcase. It contains the bomb"

    # TODO
    scene romance_start with dissolve
    narrator "The flight attendant's face pales as she processes the gravity of the situation."
    flight_attendant "You... you can't be serious."
    cooper "I am. And I need you to deliver this message to the authorities."
    narrator "She hesitates, her hands trembling slightly."
    narrator "For the success of your plan, you need her cooperation."
    narrator "You think about how to persuade her."
    menu:
        "Persuade her with a firm tone":
            narrator "You speak firmly, your voice steady."
            cooper "Listen, I know this is hard to believe, but I need you to trust me. If you do this, no one will get hurt."
        "Appeal to her sense of duty":
            narrator "You soften your tone, appealing to her sense of duty."
            cooper "I understand this is frightening, but you have a responsibility to ensure the safety of everyone on this plane. Please help me."
        "Make a joke to lighten the mood":
            $ romance = True
            narrator "You try to lighten the mood with a joke, hoping to ease her tension."
            cooper "Well, I guess this isn't the kind of in-flight entertainment you expected, huh?"
            narrator "She gives a weak smile, but you can see the fear in her eyes."
    
    narrator "Regardless of your approach, she nods, her expression serious."
    scene attendant_go_cockpit with dissolve
    flight_attendant "What do you want me to do?"
    cooper "I need you to deliver this message to the authorities. Tell them I want the money and supplies ready when we land."
    flight_attendant "And if they don't comply?"
    cooper "Then this plane goes down."
    narrator "She nods, her face pale but determined."

    scene cockpit_pilot with dissolve
    narrator "The flight attendant walks to the cockpit, her heels clicking on the floor."
    play sound footsteps fadeout 2
    narrator "You can hear her speaking to the pilot, her voice steady despite the situation."
    pilot "What does he want?"
    flight_attendant "He wants $200,000 in cash, four parachutes, and a fuel truck ready in Seattle. He has a bomb."
    pilot "A bomb? Is he serious?"
    flight_attendant "He seems to be. He handed me a note with all the details."
    narrator "The pilot's face pales as he processes the information."
    pilot "We need to inform the authorities immediately. This is a serious situation."
    narrator "The flight attendant nods, her expression resolute."
    scene plane_land with dissolve
    narrator "The plane lands safely in Seattle, the tension palpable."
    scene attendant_offboard with dissolve
    narrator "The flight attendant disembarks, her expression a mix of fear and determination."
    play sound footsteps fadeout 2
    narrator "You watch her go, knowing that the next steps are crucial."
    scene attendant_get_ransom with dissolve
    narrator "The authorities prepare the ransom, and the flight attendant returns to the plane."
    scene plane_takeoff with dissolve
    narrator "The plane takes off again, the atmosphere tense but hopeful."
    scene attendant_hand_parachute with dissolve
    narrator "The flight attendant hands you the parachutes, her hands shaking slightly."
    flight_attendant "The money is ready. We can proceed with the exchange."
    cooper "Thank you. Now, I need you to stay calm and follow my instructions."
    if romance:
        scene romance_end with dissolve
        narrator "You exchange a brief, understanding glance with the flight attendant, a silent agreement forged in the chaos."
        narrator "She passes you note, and heads back to the cockpit."
    else:
        narrator "You nod, your expression serious."
    narrator "The flight attendant returns to the cockpit, her face pale but determined."
    narrator "You prepare for the next phase of your plan, knowing that every detail matters."
