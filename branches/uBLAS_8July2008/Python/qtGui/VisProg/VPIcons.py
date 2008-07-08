## Automatically adapted for numpy.oldnumeric Jun 27, 2008 by -c

#
#  Copyright (C) 2002  Greg Landrum and Rational Discovery LLC
#    All Rights Reserved
#

# icons for use in the visual programming GUI
filterIcon_xpmData=[
"64 64 5 1",
" 	c #FFFFFFFFFFFF",
".	c #000000000000",
",	c #666666666666",
"X	c #00000000FFFF",
"o	c #FFFF5555FFFF",
"                                                                ",
"                                                                ",
"   ....... . ..                                       XXXXX     ",
"    .    .    .                                      XX,,,,X    ",
"    .         .  .                                 XXX,,,,,,X   ",
"    .   . ..  . ....  ..  . ..                   XXX,,,,,,,,X   ",
"    .....  .  .  .   .  .  ..                   XX,,,,,,,,,,X   ",
"    .   .  .  .  .   ....  .                  XX,,,,,,,,,,,,X   ",
"    .      .  .  .   .     .                XXX,,,,,,,,,,,,,X   ",
"    .      .  .  .   ..  . .              XXX ,,,,,,,,,,,,,XX   ",
"   ....   ......  ..  ... ...            XX  ,,,,,,,,,,,,XXX    ",
"                                       XXX  ,,,,,,,,,,,,XX      ",
"                                      XX    ,,,,,,,,,,XXX       ",
"                                     XXX   ,,,,,,,,,XXX         ",
"                                     X XX ,,,,,,,,XXX           ",
"                                     X  X ,,,,,,,XX             ",
"                                     XX  X,,,,,XXX              ",
"                                      X  X,,,XXX                ",
"                                      XX X,XXX                  ",
"                                       XXXXX                    ",
"                                        ,XX                     ",
"                                        ,                       ",
"                                       ,,                       ",
"                                       ,,,                      ",
"                              XXXXXXXXXX,XXXXXXX                ",
"                          XXXX                  XXXX            ",
"                        XX              ,           XX          ",
"                       X               ,,,            X         ",
"                       XXX              ,           XXX         ",
"                        X XXXX                  XXXX X          ",
"                         X    XXXXXXXXXXXXXXXXXX    X           ",
"                          X                        X            ",
"                           X       ,,,,,,,,       X             ",
"                            X   ,,,,,,,,,,,,,,   X              ",
"                             X,,,,,,,,,,,,,,,,,,X               ",
"                              X,,,,,,,,,,,,,,,,X                ",
"                               X,,,,,,,,,,,,,,X                 ",
"                                X,,,,,,,,,,,,X                  ",
"                                 X,,,,,,,,,,X                   ",
"                                  X,,,,,,,,X                    ",
"                                   X,,,,,,X                     ",
"                                    X,,,,X                      ",
"                                     X,,X                       ",
"                                     XooX                       ",
"                                     XooX                       ",
"                                     XooX                       ",
"                                     XooX                       ",
"                                     XooX                       ",
"                                     XooX                       ",
"                                     XooX                       ",
"                                     XooX                       ",
"                                     XooX                       ",
"                                     XooX                       ",
"                                     XooX                       ",
"                                     XooX                       ",
"                                     XooX                       ",
"                                     XooX                       ",
"                                     XooX                       ",
"                                      XX                        ",
"                                      oo                        ",
"                                      oo                        ",
"                                       o                        ",
"                                      o                         ",
"                                       o                        "]
rackIcon_xpmData=[
"64 64 7 1",
" 	c #FFFFFFFFFFFF",
".	c #000000000000",
"X	c #00000000FFFF",
"o	c #0000FFFFFFFF",
"O	c #FFFF00000000",
"+	c #FFFF0000FFFF",
"@	c #0000FFFF0000",
"                                                                ",
"          ......                         .                      ",
"           .   ..                                               ",
"           .    .                    .                          ",
"           .   ..   ..    ..    ..  ......   ..  . ..           ",
"           .....   .  .  .  .  .  .  .   .  .  .  .  .          ",
"           .  .    ....   ...  .     .   .  .  .  .  .          ",
"           .   .   .     .  .  .     .   .  .  .  .  .          ",
"           .    .  ..  . .  .  .  .  .   .  .  .  .  .          ",
"          ...   ..  ...   .. .  ..    .....  ..  ... ..         ",
"                                                                ",
"                                                                ",
"               XXX        XXX        XXX        XXX             ",
"             XXX XXX    XXX XXX    XXX XXX    XXX XXX           ",
"            XX     XX  XX     XX  XX     XX  XX     XX          ",
"            XXXX XXXX  XXXX XXXX  XXXX XXXX  XXXX XXXX          ",
"            X  XXX  X  X  XXX  X  X  XXX  X  X  XXX  X          ",
"            X       X  X       X  X       X  X       X          ",
"            X  ooo  X  X       X  X       X  X       X          ",
"            XoooooooX  X       X  X       X  X  OOO  X          ",
"            XoooooooX  X       X  X       X  XOOOOOOOX          ",
"            XoooooooX  X  +++  X  X       X  XOOOOOOOX          ",
"            XoooooooX  X+++++++X  X       X  XOOOOOOOX          ",
"         ...XoooooooX..X+++++++X..X  @@@  X..XOOOOOOOX........  ",
"        .   XoooooooX  X+++++++X  X.flat@@@X  XOOOOOOOX      ..  ",
"       .    XoooooooX  X+++++++X  X.flat@@@X  XOOOOOOOX     ..   ",
"      .     XXoooooXX  XX+++++XX  XX.flat@XX  XXOOOOOXX    ..    ",
"    ..       XXXoXXX    XXX+XXX    XXX@XXX    XXXOXXX    ..     ",
"   .           XXX        XXX        XXX        XXX     ...     ",
"  .                                                    .. .     ",
"  ....................................................... .     ",
"   .................................................... . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . ......XoooooooX..X+++++++X..X.flat@@@X..XOOOOO. .... ....  ",
"    . ..  . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . ....  ",
"    . .   ..XXoooooXX  XX+++++XX  XX.flat@XX  XXOOOO. .   ....   ",
"    . .    . XXXoXXX    XXX+XXX    XXX@XXX    XXXOO. .   ...    ",
"    . .        XXX        XXX        XXX        XXX. .  ...     ",
"    . .                                            . . ...      ",
"   ......................................................       ",
"  ......................................................        ",
"                                                                ",
"                                                                ",
"                                                                ",
"                                                                ",
"                                                                "]
reactionIcon_xpmData=[
"64 64 6 1",
" 	c #FFFFFFFFFFFF",
".	c #000000000000",
"X	c #00000000FFFF",
"o	c #FFFF0000FFFF",
"O	c #0000FFFFFFFF",
"+	c #0000FFFF0000",
"                                                                ",
"                                                                ",
"           ......                         .                     ",
"            .   ..                                              ",
"            .    .                    .                         ",
"            .   ..   ..    ..    ..  ......   ..  . ..          ",
"            .....   .  .  .  .  .  .  .   .  .  .  .  .         ",
"            .  .    ....   ...  .     .   .  .  .  .  .         ",
"            .   .   .     .  .  .     .   .  .  .  .  .         ",
"            .    .  ..  . .  .  .  .  .   .  .  .  .  .         ",
"           ...   ..  ...   .. .  ..    .....  ..  ... ..        ",
"                                                                ",
"                                                                ",
"                                                                ",
"          XXXXX                                   XXXXX         ",
"         XooooXX                                 XXOOOOX        ",
"        XooooooXXX                             XXXOOOOOOX       ",
"        XooooooooXXX                         XXXOOOOOOOOX       ",
"        XooooooooooXX                       XXOOOOOOOOOOX       ",
"        XooooooooooooXX                   XXOOOOOOOOOOOOX       ",
"        XoooooooooooooXXX               XXXOOOOOOOOOOOOOX       ",
"        XXooooooooooooo XXX           XXX OOOOOOOOOOOOOXX       ",
"         XXXoooooooooooo  XX         XX  OOOOOOOOOOOOXXX        ",
"           XXoooooooooooo  XXX     XXX  OOOOOOOOOOOOXX          ",
"            XXXoooooooooo    XX   XX    OOOOOOOOOOXXX           ",
"              XXXooooooooo   XX  XXX   OOOOOOOOOXXX             ",
"                XXXoooooooo X X  X XX OOOOOOOOXXX               ",
"                  XXooooooo X X  X  X OOOOOOOXX                 ",
"                   XXXoooooX  X  XX  XOOOOOXXX                  ",
"                     XXXoooX  X   X  XOOOXXX                    ",
"                       XXXoX X    XX XOXXX                      ",
"                         XXXXX     XXXXX                        ",
"                          XXo       OXX                         ",
"                            o       O                           ",
"                            oo     OO                           ",
"                           ooo     OOO                          ",
"                           ooo     OOO                          ",
"                            oXXXXXXXO                           ",
"                          XXX       XXX                         ",
"                        XX             XX                       ",
"                        XX             XX                       ",
"                        X XXX       XXX X                       ",
"                        X   XXXXXXXXX   X                       ",
"                        X               X                       ",
"                        X   XX          X                       ",
"                        X    X          X                       ",
"                        X               X                       ",
"                        X   XX    XX    X                       ",
"                        X    X+++X  X   X                       ",
"                        X +++++++X  X++ X                       ",
"                        X+++XX++ +XX++++X                       ",
"                        X++++X++++++++++X                       ",
"                        X+ ++++ ++++++ +X                       ",
"                        X+++XX++++++ +++X                       ",
"                        X++++XOOOOOOO+++X                       ",
"                        X+OOO+++++++OOO+X                       ",
"                        XXX++++++ ++++XXX                       ",
"                          XXX+++++++XXX                         ",
"                            XXXXXXXXX                           ",
"                                                                ",
"                                                                ",
"                                                                ",
"                                                                ",
"                                                                "]
sinkIcon_xpmData=[
"64 64 4 1",
" 	c #FFFFFFFFFFFF",
".	c #000000000000",
"X	c #00000000FFFF",
"o	c #0000FFFFFFFF",
"                                                                ",
"                                                                ",
"             ... .                                              ",
"            .   ..                                              ",
"            .    .  .                                           ",
"             ..    ....  ..  . ..  ..    ....  ..               ",
"              ...   .   .  .  ..  .  .  .  .  .  .              ",
"                ..  .   .  .  .    ...  .  .  ....              ",
"            .    .  .   .  .  .   .  .  ...   .                 ",
"            ..  ..  .   .  .  .   .  .   .    ..  .             ",
"            . ...    ..  ..  ...   .. .  ...   ...              ",
"                                        .   .                   ",
"                                        .   .                   ",
"                                         ...                    ",
"                                                                ",
"                                                                ",
"                                                                ",
"                                                                ",
"                                                                ",
"                                                                ",
"                                                                ",
"                XXXXX      XXXXX      XXXXX      XXXXX          ",
"               XXXXXXX    XXXXXXX    XXXXXXX    XXXXXXX         ",
"              XXXXXXXXX  XXXXXXXXX  XXXXXXXXX  XXXXXXXXX        ",
"              XXXXXXXXX  XXXXXXXXX  XXXXXXXXX  XXXXXXXXX        ",
"              XXXXXXXXX  XXXXXXXXX  XXXXXXXXX  XXXXXXXXX        ",
"              XoXXXXXoX..XoXXXXXoX..XoXXXXXoX..XoXXXXXoX.....   ",
"              XoooooooX  XoooooooX  XoooooooX  XoooooooX   ..   ",
"              XoooooooX  XoooooooX  XoooooooX  XoooooooX   ..   ",
"            XXXXXoooooXXXXXXoooooXXXXXXoooooXXXXXXoooooX  . .   ",
"           XXXXXXXooooXXXXXXXooooXXXXXXXooooXXXXXXXooooX  . .   ",
"          XXXXXXXXXooXXXXXXXXXooXXXXXXXXXooXXXXXXXXXooX   . .   ",
"          XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX   .  .   ",
"          XXXXXXXXX  XXXXXXXXX  XXXXXXXXX  XXXXXXXXX     .  .   ",
"          XoXXXXXoX  XoXXXXXoX  XoXXXXXoX  XoXXXXXoX    .   .   ",
"          XoooooooX  XoooooooX  XoooooooX  XoooooooX    .   .   ",
"          XoooooooX  XoooooooX  XoooooooX  XoooooooX    .  .    ",
"          XoooooooX  XoooooooX  XoooooooX  XoooooooX   .   .    ",
"        XXXXXoooooXXXXXXoooooXXXXXXoooooXXXXXXoooooX   .   .    ",
"       XXXXXXXooooXXXXXXXooooXXXXXXXooooXXXXXXXoooX   .   .     ",
"      XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX    .   .     ",
"     .XXXXXXXXX  XXXXXXXXX  XXXXXXXXX  XXXXXXXXX     .   .      ",
"     .XXXXXXXXX  XXXXXXXXX  XXXXXXXXX  XXXXXXXXX     .   .      ",
"     .XoXXXXXoX  XoXXXXXoX  XoXXXXXoX  XoXXXXXoX     .  .       ",
"    . XoooooooX  XoooooooX  XoooooooX  XoooooooX    .   .       ",
"    . XoooooooX  XoooooooX  XoooooooX  XoooooooX    .   .       ",
"    . XoooooooX  XoooooooX  XoooooooX  XoooooooX   .   .        ",
"   .  XoooooooX  XoooooooX  XoooooooX  XoooooooX   .   .        ",
"   .   XoooooX    XoooooX    XoooooX    XoooooX   .   .         ",
"   .    XXXXX      XXXXX      XXXXX      XXXXX    .   .         ",
"  .                                              .   .          ",
"  .                                              .   .          ",
"  ................................................   .          ",
"  .                                              .  .           ",
"  .                                              .  .           ",
"  .                                              . .            ",
"  .                                              . .            ",
"  .                                              ..             ",
"  .                                              ..             ",
"  ................................................              ",
"                                                                ",
"                                                                ",
"                                                                ",
"                                                                "]
supplyIcon_xpmData=[
"64 64 7 1",
" 	c #FFFFFFFFFFFF",
".	c #000000000000",
"X	c #00000000FFFF",
"o	c #0000FFFFFFFF",
"O	c #FFFF00000000",
"+	c #FFFF0000FFFF",
"@	c #0000FFFF0000",
"                                                                ",
"                 ... .                   ..                     ",
"                .   ..                    .                     ",
"                .    .                    .                     ",
"                 ..    .. .. ....  ....   . ..  ..              ",
"                  ...   .  .  .  .  .  .  .  .  .               ",
"                    ..  .  .  .  .  .  .  .  . ..               ",
"                .    .  .  .  .  .  .  .  .  . .                ",
"                ..  ..  .  .  .  .  .  .  .   ..                ",
"                . ...    .. . ...   ...  ...  .                 ",
"                              .     .         .                 ",
"                              .     .        .                  ",
"                             ...   ...      ..                  ",
"                                                                ",
"               XXX        XXX        XXX        XXX             ",
"             XXX XXX    XXX XXX    XXX XXX    XXX XXX           ",
"            XX     XX  XX     XX  XX     XX  XX     XX          ",
"            XXXX XXXX  XXXX XXXX  XXXX XXXX  XXXX XXXX          ",
"            X  XXX  X  X  XXX  X  X  XXX  X  X  XXX  X          ",
"            X       X  X       X  X       X  X       X          ",
"            X  ooo  X  X       X  X       X  X       X          ",
"            XoooooooX  X       X  X       X  X  OOO  X          ",
"            XoooooooX  X       X  X       X  XOOOOOOOX          ",
"            XoooooooX  X  +++  X  X       X  XOOOOOOOX          ",
"            XoooooooX  X+++++++X  X       X  XOOOOOOOX          ",
"         ...XoooooooX..X+++++++X..X  @@@  X..XOOOOOOOX........  ",
"        .   XoooooooX  X+++++++X  X.flat@@@X  XOOOOOOOX      ..  ",
"       .    XoooooooX  X+++++++X  X.flat@@@X  XOOOOOOOX     ..   ",
"      .     XXoooooXX  XX+++++XX  XX.flat@XX  XXOOOOOXX    ..    ",
"    ..       XXXoXXX    XXX+XXX    XXX@XXX    XXXOXXX    ..     ",
"   .           XXX        XXX        XXX        XXX     ...     ",
"  .                                                    .. .     ",
"  ....................................................... .     ",
"   .................................................... . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . .   . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . .     ",
"    . ......XoooooooX..X+++++++X..X.flat@@@X..XOOOOO. .... ....  ",
"    . ..  . XoooooooX  X+++++++X  X.flat@@@X  XOOOOO. .  . ....  ",
"    . .   ..XXoooooXX  XX+++++XX  XX.flat@XX  XXOOOO. .   ....   ",
"    . .    . XXXoXXX    XXX+XXX    XXX@XXX    XXXOO. .   ...    ",
"    . .        XXX        XXX        XXX        XXX. .  ...     ",
"    . .                                            . . ...      ",
"   ......................................................       ",
"  ......................................................        ",
"                                                                ",
"                                                                ",
"                                                                "]
