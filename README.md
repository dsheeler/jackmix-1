# JackMix

## Authors

| Author                               | Responsibility           |
|--------------------------------------|--------------------------|
| Arnold Krille <arnold@arnoldarts.de> | Almost everything.       |
| Nick Bailey <nick@n-ism.org>         | MIDI control extensions. |
|                                      | Qt5 port attempt.        |
| Daniel Sheeler                       | De-zippering             |

---
See:

 * INSTALL for installation instructions;
 * AUTHORS for contact information;
 * NEWS    for new features in this version.


JackMix is a matrix mixer allowing p input channels to be mixed into
q output channels by a matrix of faders. It relies on the Jack framework
to route these inputs and outputs.

There's a video about Nick using jackmix at [https://vimeo.com/75655401]

This version of jackmix is enhanced for compatibility with an external
MIDI control surface and compiles for qt5.

As of 15th June 2022, Lash support (session management) has been withdrawn.
You can save and GUI settings to configurations to xml files
using the File menu.

Nick Bailey <nick@n-ism.org>
