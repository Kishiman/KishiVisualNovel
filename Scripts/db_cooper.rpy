# Script Title: D.B. Cooper Hijacking Visual Novel - ACT 1
# Assume necessary images and sounds are loaded as per the resources section.

# Define Characters

define narrator = Character("")
define cooper = Character("Cooper", color="#FF0000")  # D.B. Cooper is the protagonist
define flight_attendant = Character("Flight Attendant", color="#00BFFF")
define pilot = Character("Pilot", color="#FFA500")

# Images
image bg_airport = "DBCooper/bg_airport"
image bg_airplane = "DBCooper/bg_airplane"
image bg_briefcase = "DBCooper/bg_briefcase"
image bg_note = "DBCooper/bg_note"
image flight_attendant = "DBCooper/airplane_attendent"

# Sounds and Music
sound engine = "Lemmino/Aloft_BGM"
sound footsteps = "pixabay/footsteps-in-a-hallway"
sound tense_music = "Lemmino/Aloft_BGM"

# Scene 1: Onboarding at the Airport
label start:
    play music tense_music
    scene bg_airport with fade
    play sound footsteps loop
    
    cooper "It's November 24, 1971. Northwest Orient Flight 305. The ticket cost $20."
    
    narrator "You step through the crowded airport, holding a briefcase in one hand and a ticket to Seattle in the other."

    stop sound

    scene bg_airplane with fade
    play music engine loop
    
    narrator "You board the Boeing 727. The hum of the engines vibrates beneath your feet as you make your way to seat 18C."
    
    narrator "You sit down, adjusting your tie and lighting a cigarette. You glance around the cabin, watching passengers find their seats."

    menu:
        "Observe the passengers":
            jump observe_passengers
        "Review the plan":
            jump review_plan

# Option 1: Observing the passengers
label observe_passengers:
    narrator "You watch the other passengers boarding. Businessmen, families, and a few lone travelers."
    narrator "They have no idea what's about to happen."

    jump review_plan

# Option 2: Reviewing the plan
label review_plan:
    narrator "The plan is simple. Once the plane is airborne, you'll pass a note to the flight attendant."
    narrator "Inside your briefcase is the key—a bomb. Or at least, what looks like one."

    menu:
        "Check the briefcase":
            jump check_briefcase
        "Wait for the plane to take off":
            jump wait_takeoff

# Checking the Briefcase (Optional but immersive)
label check_briefcase:
    scene bg_briefcase with dissolve
    
    narrator "You quietly open the briefcase under your seat. Inside, wires, red sticks, and a battery are neatly arranged."
    narrator "It looks convincing enough to fool anyone."

    cooper "Perfect."

    jump wait_takeoff

# Plane takes off and the action begins
label wait_takeoff:
    scene bg_airplane with fade
    narrator "The plane takes off, lifting into the cloudy skies. You feel the weight of what's about to happen."
    narrator "It's time."

    menu:
        "Call the flight attendant":
            jump call_flight_attendant
        "Wait a little longer":
            jump wait_longer

# Calling the Flight Attendant
label call_flight_attendant:
    show flight_attendant with dissolve
    
    flight_attendant "Sir, may I get you something?"
    
    narrator "You hand her the folded note you prepared earlier."
    
    menu:
        "Let her walk away with the note":
            jump let_walk_away
        "Tell her to read it now":
            jump tell_read_now

# Waiting Too Long (Bad Ending 1)
label wait_longer:
    narrator "You hesitate, watching as the passengers settle in. Your heart pounds."
    narrator "Suddenly, you realize you've waited too long."
    narrator "The flight attendant passes by, but you say nothing."

    narrator "The opportunity slips through your fingers."

    narrator "Game Over - Bad Ending 1: You missed your chance to initiate the hijacking."
    return

# Flight Attendant Walks Away with the Note
label let_walk_away:
    narrator "She glances at the note but doesn't open it immediately. You watch as she walks toward the front of the plane."
    narrator "Now, you wait."

    jump alert_cockpit

# Telling Her to Read It Now
label tell_read_now:
    narrator "Before she walks away, you lean in slightly, keeping your voice low."
    
    cooper "Miss, you might want to read that now."

    narrator "Her smile falters as she opens the note. Her eyes widen in shock."

    show bg_note with fade
    narrator "{i}I have a bomb in my briefcase. I want $200,000 in cash, four parachutes, and a fuel truck ready in Seattle. No funny business, or I'll do it.{/i}"

    flight_attendant "(Whispers) Oh my god…"

    narrator "She stiffens, nods, and quickly walks toward the cockpit."

    jump alert_cockpit

# The Flight Attendant Alerts the Cockpit
label alert_cockpit:
    scene bg_airplane with fade
    narrator "You watch her disappear into the cockpit to speak with the pilots. The tension in the cabin increases, but outwardly, nothing seems amiss."
    
    narrator "You glance down at the briefcase by your side. It's all going according to plan."
    
    narrator "Now, you wait for the negotiations to begin."

    menu:
        "Light a cigarette":
            jump light_cigarette
        "Review the demands in your head":
            jump review_demands

# Light a Cigarette
label light_cigarette:
    narrator "You light another cigarette, watching the smoke curl up toward the ceiling."
    narrator "You savor the moment—everything is still under your control."

    jump review_demands

# Review the Demands
label review_demands:
    narrator "Your demands are simple: $200,000 in cash, four parachutes, and a fuel truck in Seattle."
    narrator "If they comply, you'll release the passengers and take off again."

    narrator "The money is the key—small bills, unmarked. It will give you a head start."

    jump next_phase

# Transition to the next act
label next_phase:
    narrator "Time drags on as you wait for the cockpit to respond. The hum of the engines continues, but the tension in the air is palpable."
    
    narrator "It's only a matter of time before the pilots contact air traffic control to begin negotiations."
    
    narrator "Everything depends on the next few moments."

    # To be continued in ACT 2...
    return
