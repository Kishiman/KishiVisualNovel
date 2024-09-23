define aug = Character('Augustina', color="#c8ffc8")

label start:
    show Augustina
    # displaying dress and neutral
    voice "Voice/cicifyre/BrightFemale/01-good_afternoon"
    aug "I like this dress."

    show Augustina happy
    # auto-defined in the auto group
    voice "Voice/cicifyre/BrightFemale/01-okay"
    aug "But what I like even more..."

    show Augustina uniform happy
    voice "Voice/cicifyre/BrightFemale/01-laughter"
    # uniform replaces dress, neutral replaces happy
    aug "Is this uniform !"
