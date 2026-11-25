Little protection I done a while ago for Cartel and xbGuards GTA IV menu.

Hooks into xams SecRegLookup function and caches the MID, compares it against the one coming in from players joining the lobby and if host will kick them.

Code is quite messy and could be made into an actual better system to avoid false positives or potentially pair with an offhost kick to prevent it when offhost.
