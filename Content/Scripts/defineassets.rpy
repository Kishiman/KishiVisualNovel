# Declaration script of a billion assets.

init:
    $ meet = 0
    $ spa = 0
    $ climb = 0

    # TODO
        # for some reason renpy isnt picking this specific image up so I need to define it omg
    image mansion2F = 'images/bgs/mansion2f.png'

# Define Characters.
    define ep = Character ("Echo Pine", color="e3a856")
    define aw = Character("Ashley Westbridge", color="8e8a79")
    define sk = Character("Sei Konno", color="7d2a4c")
    define pe = Character("Pandora Espère", color="99b36e")
    define mv = Character("Missy Vesper", color="b33a4e")
    define cs = Character("Cyrus Suryc", color="ef9e94")
    define ac = Character("Art Campopiano", color="5abdc4")
    define fs = Character("Forte Solo", color="8f8f8f")
    define ko = Character("Kanon Ota", color="586782")
    define lt = Character("Lyle Termina", color="635096")
# Thoughts.
    define th = Character(None, what_color="#419BE6", what_prefix='(', what_suffix=')')
# Unknown voice.
    define u = Character("???", color="fff")
# Text or data.
    define data = Character(None, what_color="#68D02C", what_prefix='"', what_suffix='"')
    define AQ = Character("???", what_color="#ff4e4e", what_prefix='<', what_suffix='>')

# Interesting text color, {color=#D7B213}_{/color}

# Press mechanic?
# ep "I'm {a=jump:MOREecho}{u}{color=#23BED1}Echo Pine{/a}{/u}{/color}."

# Define character sprites.
    image ep = "images/characters/echo.png"
    image ep scr = "images/characters/echo scr.png"

    image aw = "images/characters/ash.png"
    image aw fb = "images/characters/ash fb.png"
    image aw sad = "images/characters/ash sad.png"
    image aw shk = "images/characters/ash shock.png"

    image sk = "images/characters/sei.png"
    image sk q = "images/characters/sei quiet.png"
    image sk shk = "images/characters/sei shk.png"

    image pe = "images/characters/dora.png"
    image pe hope = "images/characters/dora hope.png"
    image pe srp = "images/characters/dora srp.png"

    image mv = "images/characters/missy.png"
    image mv cr = "images/characters/missy cross.png"
    image mv sm = "images/characters/missy smile.png"
    image mv th = "images/characters/missy think.png"

    image cs = "images/characters/cyrus.png"
    image cs sm = "images/characters/cyrus smile.png"
    image cs srs = "images/characters/cyrus srs.png"

    image ac = "images/characters/art.png"
    image ac happy = "images/characters/art happy.png"
    image ac scr = "images/characters/art scared.png"

    image fs = "images/characters/forte.png"
    image fs shk = "images/characters/forte shk.png"
    image fs str = "images/characters/forte stress.png"

    image ko = "images/characters/kanon.png"
    image ko m = "images/characters/kanon mad.png"
    image ko q = "images/characters/kanon quiet.png"

    image lt = "images/characters/lyle.png"
    image lt eag = "images/characters/lyle eager.png"
    image lt shk = "images/characters/lyle shk.png"
    image lt th = "images/characters/lyle think.png"

# Define audio.
    define audio.beautyday = "music/1-04 Beautiful Days.mp3"
    define audio.beautydead = "music/1-05 Beautiful Dead.mp3"
    define audio.monoextra = "music/1-07 Mr. Monokuma's Extracurricular Lesson.mp3"
    define audio.box15 = "music/1-08 BOX 15.mp3"
    define audio.junkfood = "music/1-09 Junk Food for a Dashing Youth.mp3"
    define audio.distrust = "music/1-10 DISTRUST.mp3"
    define audio.apologies = "music/2-14 All All Apologies.mp3"
    define audio.despairschool = "music/1-23 Welcome Despair School.mp3"
    define audio.beautymorning = "music/1-26 Beautiful Morning.mp3"
# define audio. = "music/.mp3"

# Define SFX.
    define audio.alarm = "sound/alarm.wav"
    define audio.atk1 = "sound/atk1.wav"
    define audio.atk2 = "sound/atk2.wav"
    define audio.atk3 = "sound/atk3.wav"
    define audio.beaming = "sound/beaming.wav"
    define audio.BREAK = "sound/BREAK.wav"
    define audio.clock = "sound/clock.wav"
    define audio.elec = "sound/elec.wav"
    define audio.exclaim = "sound/exclaim.wav"
    define audio.gavel = "sound/gavel.wav"
    define audio.heartrush = "sound/heartrush.wav"
    define audio.LOAD = "sound/LOAD.wav"
    define audio.megaton = "sound/megaton.wav"
    define audio.shock = "sound/shock.wav"
    define audio.slam = "sound/slam.wav"
    define audio.slash = "sound/slash.wav"
    define audio.slashbreak = "sound/slashbreak.wav"
    define audio.ton = "sound/ton.wav"

# Define effects.
    define flash = Fade(.10, 0.0, .10, color="fff")
    define pinkflash = Fade(.10, 0.0, .10, color="fb3ebe")
    define d = Dissolve(.50, alpha=False, time_warp=None)
    define wiper = CropMove(0.15, mode="wiperight")
    transform right:
        yalign 1.0
        xalign 0.90
    transform left:
        yalign 1.0
        xalign 0.10
# Particular transforms for furthest sides.
    transform rightx:
        yalign 1.0
        xalign 1.0
    transform leftx:
        yalign 1.0
        xalign 0.0

# Define transitions.
    $ shot = ImageDissolve("images/assets/shot.png", 0.8, 10)
    $ burst = ImageDissolve("images/assets/burst.png", 0.5, 8)
    $ pixel = ImageDissolve("images/assets/pixels.png", 1.0, 8)
    $ slide = ImageDissolve("images/assets/slide.png", 1.25, 4)

# Define screens.
    screen ctc(arg=None):
        if not config.skipping:
            on 'hide' action Play('sound', 'sound/DRCTC.wav')

# Call functions.
label gamestart:
    stop music fadeout 2.0
    scene black
    with Pause (2.0)

    return

label gameend:
    window hide
    play sound '<from 8 to 15>sound/chapter.mp3'
# Change chapter card as appropriate.
    scene _ with pixel
    with Pause (7.0)
    scene black with wiper

    return


# Between scene transition
# window hide
# scene black with d
# scene mansionhall with slide
# window show
