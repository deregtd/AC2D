<?php

$blah = dir("pics/");

$imlists = array();

while ($nd = $blah->read())
{
	if ($nd == ".") continue;
	if ($nd == "..") continue;
	if ($nd == "ac2dlogo.jpg") continue;
	if ($nd == "Thumbs.db") continue;
	if (strpos($nd, "_sm") != FALSE) continue;

	$imlists[substr($nd,0,strpos($nd,"."))] = filemtime("pics/$nd");
}

asort($imlists);

$imlist = array();
foreach ($imlists as $key => $val)
	$imlist[] = $key;

$imcnt = count($imlist);
if ($imcnt & 1)
	$imcntL = intval($imcnt / 2 + 1);
else
	$imcntL = $imcnt / 2;

?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML>
<HEAD>
<META HTTP-EQUIV="Content-Type" CONTENT="text/html; charset=ISO-8859-1">
<TITLE>AC2D - The Alternate Client Project</TITLE>
<STYLE TYPE="text/css">
body {
	color: white;
	background-color: black;
}
a:link {
	color: #6060FF;
}
ul {
	margin-top: 5px;
	margin-bottom: 5px;
}
hr {
	color: #C06000;
	background-color: #C06000;
	border: 0px;
	height: 3px;
	width: 70%;
}
.sshot {
	margin: 5px 5px 5px 5px;
}
</STYLE>
</HEAD>
<BODY>

<TABLE WIDTH="100%" CELLSPACING=0 CELLPADDING=3 BORDER=0 ALIGN=CENTER>
<TR>
<TD VALIGN=TOP>
<?php
for ($i=0; $i<$imcntL; $i++)
	print "<A HREF=\"pics/{$imlist[$i]}.jpg\" TARGET=\"_blank\"><IMG BORDER=0 SRC=\"pics/{$imlist[$i]}_sm.jpg\" CLASS=\"sshot\" ALT=\"AC2D Shot\"></A><BR>";
?>
</TD>
<TD VALIGN=TOP ALIGN=CENTER>
<IMG SRC="pics/ac2dlogo.jpg" ALT="AC2D: The Alternate Client Project"><BR>
<SPAN STYLE="font:12px Verdana; color:#808080">by Akilla</SPAN>
<HR>
<SPAN STYLE="font:18px Verdana"><B>Latest Build: 10-08-2006, 4:00 AM</B></SPAN><BR>
<SPAN STYLE="font:24px Verdana"><A HREF="terrain.zip">Download Terrain Viewer</A></SPAN><BR>

<DIV STYLE="text-align:left; horizontal-align:center; width:90%; margin-top:15px">
<SPAN STYLE="font:18px Verdana">What's new recently:</SPAN>
<UL>
<LI>Added full landcell support, so building interiors and dungeons are all working
<LI>Fixed up transparency, so cell divisions are seethrough now
<LI>Fixed some texture bugs (BGR->RGB)
<LI>Added terrain textures
<LI>Added proper triangle-folding (which way the terrain quads fold)
<LI>Fixed up all the memory leaks - the massive amount of memory that the thing uses now is actual data it's needing to store... 
</UL>
</DIV>

<HR/>
<SPAN STYLE="font:24px Verdana">Terrain Viewer Instructions/Controls:</SPAN><BR>
<DIV STYLE="text-align:left; horizontal-align:center; width:95%; margin-top:10px">

<SPAN STYLE="font:18px Verdana">Installation:</SPAN>
<UL>
<LI>Download and extract the viewer to your AC:ToD directory
<LI>Run it from that directory as the active directory
</UL>

<SPAN STYLE="font:18px Verdana">Controls:</SPAN>
<UL>
<LI><B>WSAD</B> - Standard FPS movement controls
<LI><B>Right click + drag</B> - Rotate camera
<LI><B>F</B> - Toggle 0.2/0.5/1/2/4/8/16/32x movement speed
<LI><B>1 through 9</B> - Sets render radius to 2x the number - i.e. 9 is a render radius of 18, so a 36x36 drawgrid. Only do this if you have a fast computer and video card. It's set to render radius of 5 to start with...
<LI><B>Insert</B> - Resets camera to default location (over sanc IIRC) 
<LI><B>Click the Minimap</B> - Transports you to the spot you clicked
</UL>
</DIV>
<HR/>
<SPAN STYLE="font:24px Verdana">Story/Summary of Purpose:</SPAN><BR>
<DIV STYLE="text-align:left; horizontal-align:center; width:95%; margin-top:10px">
<P>AC2D started back in either late 2004 or early 2005 as AC2Dx, a project by pea to create a fully-functional console client for Asheron's Call.  This would be for things like buffbots, etc., that didn't require a graphical interface, and hence didn't need to take up any memory.  Before the project got too far, I don't actually remember what happened, but somehow pea and I started working on a new version of AC2Dx intending to be a graphical client, which was just dubbed AC2D.  Before it got too far, pea lost interest and started working on a server emulator instead, and I continued on AC2D alone.</P>

<P>I worked on and off on the project for several months.  ToD brought around several changes that were hard to deal with.  It took a couple months of periodic work before AC2D was even close to its former glory.  By fall of 2005, it was a quite capable client.  It was able to login, properly transfer worldservers, view most objects of the world, do basic landblock/object decoding and viewing, and even contained a very functional user interface built around a custom windowing system.  However, with my impending senior thesis to complete, I ran out of energy to keep working on the project, and it sat around, untouched, until October 2006.  Something got me wanting to work on it again, so I picked up the project.</P>

<P>I decided that, to begin with, it would be more useful to do a #define'd version of the client that was solely a terrain flyer.  This would give me the ability to simply develop the client's rendering ability without having to login to a server each time, as well as allowing me full freedom to traverse the world to test things.  So, I roped off a section of the code into a #define that would launch me straight into a worldviewer, and added custom controls for it, and the project was reborn.  When the terrain viewer gets to a point that I'm happy with it, I will resume standard AC2D development.</P>

<P>The final purpose of AC2D is to reverse-engineer enough of the game's file formats/network protocol to make a usable alternate client to the normal Turbine one.  The interface on the normal client is horrible, the graphics are slow and dated, and the plugin system is terrible.  AC2D was designed from the ground up to have an incredibly versatile class interface for accessing all elements of the world, and with a custom windowing system, allowing a transparent and fully customizable user interface.  Where necessary, I attempt to work with turbine to ensure that the client acts as close to the real client as possible, and to do everything possible to prevent crashing any of the turbine servers.  This usually includes doing development on empty landcells, in case something happens, but to this date I do not know of any time that I have crashed a landcell.  When the client is at a point that I believe it to be fully compatibile with turbine's systems without any risk of crashing things, as well as, hopefully, getting the blessing of Turbine, I will release the full client and not just the terrain viewer portion of it.</P>

</DIV>
<HR/>
<SPAN STYLE="font:24px Verdana">Current Progress:</SPAN><BR>
<DIV STYLE="text-align:left; horizontal-align:center; width:95%; margin-top:10px">
<TABLE WIDTH="100%" CELLPADDING=3 BORDER=1 CELLSPACING=0 STYLE="font:14px Verdana">
<TR STYLE="font-size:20px">
<TD ALIGN=CENTER WIDTH="50%">File Formats</TD>
<TD ALIGN=CENTER WIDTH="50%">Network Protocol</TD>
</TR>

<TR>
<TD VALIGN=TOP>
<SPAN STYLE="font:18px Verdana">Portal.Dat:</SPAN>
<UL>
<LI>01 - Type 1 models - Fully Loading
<LI>02 - Type 2 models (Modelgroups) - Mostly loading, missing several unknowns for initial positions
<LI>03 - Animations - Somewhat working, missing several unknowns, especially for combined animations
<LI>04 - Palettes - Fully loading
<LI>05 - Texture lookups - Fully loading
<LI>06 - Textures - All formats other than JPEG loading
<LI>08 - Texture Lookups - Fully loading
<LI>0A - Sounds - Untouched
<LI>0D - Dungeon Parts - Fully loading
<LI>0E - UI text - Fully loading
<LI>0F - palette lookups - Not used
<LI>13 - Worldinfo - Using some data from it, the actual file type is undecoded
<LI>22 - String lists - Untouched
<LI>25 - More string lists - Untouched
<LI>27 - Magic Strings - Untouched
<LI>31 - Character creation strings - Untouched
</UL>

<SPAN STYLE="font:18px Verdana">Cell.Dat:</SPAN>
<UL>
<LI>FFFF - Main Landblock Heightmap - Fully loading, some bits of the type flag still not used (vegetation density, blending, road specifics)
<LI>FFFE - Landblock Object List - Mostly loading, plenty of unknowns left
<LI>0100 + xx - Landcells - Mostly loading, plenty of unknowns left, but is able to load the dungeonparts
</UL>
</TD>

<TD VALIGN=TOP>
<SPAN STYLE="font:18px Verdana">Loginserver:</SPAN>
<UL>
<LI>Currently only connects to Nov 05 server, will be updated soon
<LI>Logs in to all of the different given login servers
<LI>Properly transfers between login servers
<LI>Parses character list, MOTD, account info
<LI>No character creation/deletion support
</UL>

<SPAN STYLE="font:18px Verdana">Worldserver:</SPAN>
<UL>
<LI>Most important packets decoded aside from pack handling, trading, and general item wielding/management
<LI>Transfers worldservers properly, most of the time...
<LI>Spellcasting fully functional
<LI>Combat mode fully functional
<LI>Skills, stats, map, chat, emotes fully functional and parsed into a charinfo class
<LI>Animations, object creation, fully functional
<LI>Limited movement ability with client
</UL>
</TD>
</TR>

</TABLE>
</DIV>
<HR/>
<SPAN STYLE="font:24px Verdana">Contact:</SPAN><BR>
<DIV STYLE="text-align:left; horizontal-align:center; width:95%; margin-top:10px">
<UL>
<LI><B>IRC</B> - irc.sorcery.net - Akilla in #acdev or #ac2d
<LI><B>AIM</B> - zenacmaster
<LI><B>Email</B> - akilla[spam]@[spam]earthlink.net (see if you can figure it out)
</UL>
</DIV>
<HR/>
<SPAN STYLE="font:24px Verdana">Acknowledgments:</SPAN><BR>
<DIV STYLE="text-align:left; horizontal-align:center; width:95%; margin-top:10px">
<UL>
<LI><B>Pea</B> - For starting this project off, and for tons of help along the way with file formats/protocol RE
<LI><B>KewtSquirrel</B> - For lots of help with file formats/protocol RE
<LI><B>#acdev</B> - Random coding help/AC info
</UL>
</DIV>
<HR/>
</TD>
<TD VALIGN=TOP>
<?php
for ($i=$imcntL; $i<$imcnt; $i++)
	print "<A HREF=\"pics/{$imlist[$i]}.jpg\" TARGET=\"_blank\"><IMG BORDER=0 SRC=\"pics/{$imlist[$i]}_sm.jpg\" CLASS=\"sshot\" ALT=\"AC2D Shot\"></A><BR>";
?>
</TD>
</TABLE>

</BODY>
</HTML>
