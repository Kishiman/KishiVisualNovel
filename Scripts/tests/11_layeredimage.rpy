layeredimage augustina:
    zoom 1.4
    at recolor_transform

    always:
        "augustina_base"

    attribute base2 default

    group outfit:
        attribute dress default:
            "augustina_dress"
        attribute uniform

    group face auto:
        pos (100, 100)
        attribute neutral default

label start:
    show augustina # displaying dress and neutral
    aug "I like this dress."

    show augustina happy # auto-defined in the auto group
    aug "But what I like even more..."

    show augustina uniform -happy # uniform replaces dress, neutral replaces happy
    aug "Is this uniform !"
