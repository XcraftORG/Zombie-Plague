/**
 * ============================================================================
 *
 *  Zombie Plague Mod #3 Generation
 *
 *  File:          zombieclases.cpp
 *  Type:          Manager 
 *  Description:   Zombie classes generator.
 *
 *  Copyright (C) 2015-2018 Nikita Ushakov (Ireland, Dublin)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ============================================================================
 **/
 
/**
 * Number of max valid zombie classes.
 **/
#define ZombieClassMax 32
 
/**
 * Array handle to store zombie class native data.
 **/
ArrayList arrayZombieClasses;

/**
 * Zombie native data indexes.
 **/
enum
{
    ZOMBIECLASSES_DATA_NAME,
    ZOMBIECLASSES_DATA_INFO,
    ZOMBIECLASSES_DATA_MODEL,
    ZOMBIECLASSES_DATA_CLAW,
    ZOMBIECLASSES_DATA_GRENADE,
    ZOMBIECLASSES_DATA_HEALTH,
    ZOMBIECLASSES_DATA_SPEED,
    ZOMBIECLASSES_DATA_GRAVITY,
    ZOMBIECLASSES_DATA_KNOCKBACK,
    ZOMBIECLASSES_DATA_LEVEL,
    ZOMBIECLASSES_DATA_VIP,
    ZOMBIECLASSES_DATA_DURATION,
    ZOMBIECLASSES_DATA_COUNTDOWN,
    ZOMBIECLASSES_DATA_REGENHEALTH,
    ZOMBIECLASSES_DATA_REGENINTERVAL,
    ZOMBIECLASSES_DATA_CLAW_ID,
    ZOMBIECLASSES_DATA_GRENADE_ID,
    ZOMBIECLASSES_DATA_SOUNDDEATH,
    ZOMBIECLASSES_DATA_SOUNDHURT,
    ZOMBIECLASSES_DATA_SOUNDIDLE,
    ZOMBIECLASSES_DATA_SOUNDRESPAWN,
    ZOMBIECLASSES_DATA_SOUNDBURN,
    ZOMBIECLASSES_DATA_SOUNDATTACK,
    ZOMBIECLASSES_DATA_SOUNDFOOTSTEP,
    ZOMBIECLASSES_DATA_SOUNDREGEN
}

/**
 * Initialization of zombie classes. 
 **/
void ZombieClassesLoad(/*void*/)
{
    // No zombie classes?
    if(arrayZombieClasses == INVALID_HANDLE)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Zombie Class Validation", "No zombie classes loaded");
    }

    // Initialize model char
    static char sModel[PLATFORM_MAX_PATH];

    // Precache of the zombie classes
    int iSize = GetArraySize(arrayZombieClasses);
    for(int i = 0; i < iSize; i++)
    {
        // Validate player model
        ZombieGetModel(i, sModel, sizeof(sModel));
        if(!ModelsPlayerPrecache(sModel))
        {
            LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Model Validation", "Invalid model path. File not found: \"%s\"", sModel);
        }

        // Validate claw model
        ZombieGetClawModel(i, sModel, sizeof(sModel));
        ZombieSetClawIndex(i, ModelsWeaponPrecache(sModel));
        
        // Validate grenade model
        ZombieGetGrenadeModel(i, sModel, sizeof(sModel));
        ZombieSetGrenadeIndex(i, ModelsWeaponPrecache(sModel));
    }
}

/**
 * Creates commands for zombie classes module. Called when commands are created.
 **/
void ZombieOnCommandsCreate(/*void*/)
{
    // Hook commands
    RegConsoleCmd("zzombieclassmenu", ZombieCommandCatched, "Open the zombie classes menu.");
}

/**
 * Handles the <!zzombieclassmenu> command. Open the zombie classes menu.
 * 
 * @param clientIndex       The client index.
 * @param iArguments        The number of arguments that were in the argument string.
 **/ 
public Action ZombieCommandCatched(int clientIndex, int iArguments)
{
    // Open the zombie classes menu
    ZombieMenu(clientIndex);
}

/*
 * Zombie classes natives API.
 */

/**
 * Gets the amount of all zombie classes.
 *
 * native int ZP_GetNumberZombieClass();
 **/
public int API_GetNumberZombieClass(Handle isPlugin, int iNumParams)
{
    // Return the value 
    return GetArraySize(arrayZombieClasses);
}

/**
 * Gets the zombie class index of the client.
 *
 * native int ZP_GetClientZombieClass(clientIndex);
 **/
public int API_GetClientZombieClass(Handle isPlugin, int iNumParams)
{
    // Gets real player index from native cell 
    int clientIndex = GetNativeCell(1);

    // Return the value 
    return gClientData[clientIndex][Client_ZombieClass];
}

/**
 * Gets the zombie next class index of the client.
 *
 * native int ZP_GetClientZombieClassNext(clientIndex);
 **/
public int API_GetClientZombieClassNext(Handle isPlugin, int iNumParams)
{
    // Gets real player index from native cell 
    int clientIndex = GetNativeCell(1);

    // Return the value 
    return gClientData[clientIndex][Client_ZombieClassNext];
}

/**
 * Sets the zombie class index to the client.
 *
 * native void ZP_SetClientZombieClass(clientIndex, iD);
 **/
public int API_SetClientZombieClass(Handle isPlugin, int iNumParams)
{
    // Gets real player index from native cell 
    int clientIndex = GetNativeCell(1);

    // Gets class index from native cell
    int iD = GetNativeCell(2);

    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }
    
    // Sets next class to the client
    gClientData[clientIndex][Client_ZombieClassNext] = iD;
    
    // Return on success
    return iD;
}

/**
 * Load zombie class from other plugin.
 *
 * native int ZP_RegisterZombieClass(name, model, claw_model, gren_model, health, speed, gravity, vip, duration, countdown, death, hurt, idle, respawn, burn, attack, foot, regen)
 **/
public int API_RegisterZombieClass(Handle isPlugin, int iNumParams)
{
    // If array hasn't been created, then create
    if(arrayZombieClasses == INVALID_HANDLE)
    {
        // Create array in handle
        arrayZombieClasses = CreateArray(ZombieClassMax);
    }

    // Retrieves the string length from a native parameter string
    int maxLen;
    GetNativeStringLength(1, maxLen);
    
    // Validate size
    if(!maxLen)
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Can't register zombie class with an empty name");
        return -1;
    }
    
    // Gets zombie classes amount
    int iCount = GetArraySize(arrayZombieClasses);
    
    // Maximum amout reached ?
    if(iCount >= ZombieClassMax)
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Maximum number of zombie classes reached (%d). Skipping other classes.", ZombieClassMax);
        return -1;
    }

    // Initialize char
    char sZombieBuffer[PLATFORM_MAX_PATH];

    // Initialize array block
    ArrayList arrayZombieClass = CreateArray(ZombieClassMax);
    
    // Push native data into array
    GetNativeString(1, sZombieBuffer, sizeof(sZombieBuffer)); 
    arrayZombieClass.PushString(sZombieBuffer); // Index: 0
    GetNativeString(2, sZombieBuffer, sizeof(sZombieBuffer));   
    arrayZombieClass.PushString(sZombieBuffer); // Index: 1
    GetNativeString(3, sZombieBuffer, sizeof(sZombieBuffer));  
    arrayZombieClass.PushString(sZombieBuffer); // Index: 2
    GetNativeString(4, sZombieBuffer, sizeof(sZombieBuffer));   
    arrayZombieClass.PushString(sZombieBuffer); // Index: 3
    GetNativeString(5, sZombieBuffer, sizeof(sZombieBuffer)); 
    arrayZombieClass.PushString(sZombieBuffer); // Index: 4
    arrayZombieClass.Push(GetNativeCell(6));    // Index: 5
    arrayZombieClass.Push(GetNativeCell(7));    // Index: 6
    arrayZombieClass.Push(GetNativeCell(8));    // Index: 7
    arrayZombieClass.Push(GetNativeCell(9));    // Index: 8
    arrayZombieClass.Push(GetNativeCell(10));   // Index: 9
    arrayZombieClass.Push(GetNativeCell(11));   // Index: 10
    arrayZombieClass.Push(GetNativeCell(12));   // Index: 11
    arrayZombieClass.Push(GetNativeCell(13));   // Index: 12
    arrayZombieClass.Push(GetNativeCell(14));   // Index: 13
    arrayZombieClass.Push(GetNativeCell(15));   // Index: 14
    arrayZombieClass.Push(0);                   // Index: 15
    arrayZombieClass.Push(0);                   // Index: 16
    GetNativeString(16, sZombieBuffer, sizeof(sZombieBuffer));   
    arrayZombieClass.PushString(sZombieBuffer); // Index: 17
    GetNativeString(17, sZombieBuffer, sizeof(sZombieBuffer)); 
    arrayZombieClass.PushString(sZombieBuffer); // Index: 18
    GetNativeString(18, sZombieBuffer, sizeof(sZombieBuffer)); 
    arrayZombieClass.PushString(sZombieBuffer); // Index: 19
    GetNativeString(19, sZombieBuffer, sizeof(sZombieBuffer));
    arrayZombieClass.PushString(sZombieBuffer); // Index: 20
    GetNativeString(20, sZombieBuffer, sizeof(sZombieBuffer)); 
    arrayZombieClass.PushString(sZombieBuffer); // Index: 21
    GetNativeString(21, sZombieBuffer, sizeof(sZombieBuffer));  
    arrayZombieClass.PushString(sZombieBuffer); // Index: 22
    GetNativeString(22, sZombieBuffer, sizeof(sZombieBuffer)); 
    arrayZombieClass.PushString(sZombieBuffer); // Index: 23
    GetNativeString(23, sZombieBuffer, sizeof(sZombieBuffer));   
    arrayZombieClass.PushString(sZombieBuffer); // Index: 24

    // Store this handle in the main array
    arrayZombieClasses.Push(arrayZombieClass);

    // Return id under which we registered the class
    return GetArraySize(arrayZombieClasses)-1;
}

/**
 * Gets the name of a zombie class at a given index.
 *
 * native void ZP_GetZombieClassName(iD, sName, maxLen);
 **/
public int API_GetZombieClassName(Handle isPlugin, int iNumParams)
{
    // Gets class index from native cell
    int iD = GetNativeCell(1);

    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }
    
    // Gets string size from native cell
    int maxLen = GetNativeCell(3);

    // Validate size
    if(!maxLen)
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "No buffer size");
        return -1;
    }
    
    // Initialize name char
    static char sName[SMALL_LINE_LENGTH];
    ZombieGetName(iD, sName, sizeof(sName));

    // Return on success
    return SetNativeString(2, sName, maxLen);
}

/**
 * Gets the info of a zombie class at a given index.
 *
 * native void ZP_GetZombieClassInfo(iD, sInfo, maxLen);
 **/
public int API_GetZombieClassInfo(Handle isPlugin, int iNumParams)
{
    // Gets class index from native cell
    int iD = GetNativeCell(1);

    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }
    
    // Gets string size from native cell
    int maxLen = GetNativeCell(3);

    // Validate size
    if(!maxLen)
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "No buffer size");
        return -1;
    }
    
    // Initialize info char
    static char sInfo[BIG_LINE_LENGTH];
    ZombieGetInfo(iD, sInfo, sizeof(sInfo));

    // Return on success
    return SetNativeString(2, sInfo, maxLen);
}

/**
 * Gets the player model of a zombie class at a given index.
 *
 * native void ZP_GetZombieClassModel(iD, sModel, maxLen);
 **/
public int API_GetZombieClassModel(Handle isPlugin, int iNumParams)
{
    // Gets class index from native cell
    int iD = GetNativeCell(1);

    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }
    
    // Gets string size from native cell
    int maxLen = GetNativeCell(3);

    // Validate size
    if(!maxLen)
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "No buffer size");
        return -1;
    }
    
    // Initialize model char
    static char sModel[PLATFORM_MAX_PATH];
    ZombieGetModel(iD, sModel, sizeof(sModel));

    // Return on success
    return SetNativeString(2, sModel, maxLen);
}

/**
 * Gets the knife model of a zombie class at a given index.
 *
 * native void ZP_GetZombieClassClaw(iD, sModel, maxLen);
 **/
public int API_GetZombieClassClaw(Handle isPlugin, int iNumParams)
{
    // Gets class index from native cell
    int iD = GetNativeCell(1);

    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }
    
    // Gets string size from native cell
    int maxLen = GetNativeCell(3);

    // Validate size
    if(!maxLen)
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "No buffer size");
        return -1;
    }
    
    // Initialize model char
    static char sModel[PLATFORM_MAX_PATH];
    ZombieGetClawModel(iD, sModel, sizeof(sModel));

    // Return on success
    return SetNativeString(2, sModel, maxLen);
}

/**
 * Gets the grenade model of a zombie class at a given index.
 *
 * native void ZP_GetZombieClassGrenade(iD, sModel, maxLen);
 **/
public int API_GetZombieClassGrenade(Handle isPlugin, int iNumParams)
{
    // Gets class index from native cell
    int iD = GetNativeCell(1);

    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }
    
    // Gets string size from native cell
    int maxLen = GetNativeCell(3);

    // Validate size
    if(!maxLen)
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "No buffer size");
        return -1;
    }
    
    // Initialize model char
    static char sModel[PLATFORM_MAX_PATH];
    ZombieGetGrenadeModel(iD, sModel, sizeof(sModel));

    // Return on success
    return SetNativeString(2, sModel, maxLen);
}

/**
 * Gets the health of the zombie class.
 *
 * native int ZP_GetZombieClassHealth(iD);
 **/
public int API_GetZombieClassHealth(Handle isPlugin, int iNumParams)
{
    // Gets class index from native cell
    int iD = GetNativeCell(1);
    
    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }
    
    // Return value
    return ZombieGetHealth(iD);
}

/**
 * Gets the speed of the zombie class.
 *
 * native float ZP_GetZombieClassSpeed(iD);
 **/
public int API_GetZombieClassSpeed(Handle isPlugin, int iNumParams)
{
    // Gets class index from native cell
    int iD = GetNativeCell(1);
    
    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }
    
    // Return the value (Float fix)
    return view_as<int>(ZombieGetSpeed(iD));
}

/**
 * Gets the gravity of the zombie class.
 *
 * native float ZP_GetZombieClassGravity(iD);
 **/
public int API_GetZombieClassGravity(Handle isPlugin, int iNumParams)
{
    // Gets class index from native cell
    int iD = GetNativeCell(1);
    
    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }
    
    // Return the value (Float fix)
    return view_as<int>(ZombieGetGravity(iD));
}

/**
 * Gets the knockback of the zombie class.
 *
 * native float ZP_GetZombieClassKnockBack(iD);
 **/
public int API_GetZombieClassKnockBack(Handle isPlugin, int iNumParams)
{
    // Gets class index from native cell
    int iD = GetNativeCell(1);
    
    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }
    
    // Return the value (Float fix)
    return view_as<int>(ZombieGetKnockBack(iD));
}

/**
 * Gets the level of the zombie class.
 *
 * native int ZP_GetZombieClassLevel(iD);
 **/
public int API_GetZombieClassLevel(Handle isPlugin, int iNumParams)
{
    // Gets class index from native cell
    int iD = GetNativeCell(1);
    
    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }
    
    // Return value
    return ZombieGetLevel(iD);
}

/**
 * Check the access of the zombie class.
 *
 * native bool ZP_IsZombieClassVIP(iD);
 **/
public int API_IsZombieClassVIP(Handle isPlugin, int iNumParams)
{
    // Gets class index from native cell
    int iD = GetNativeCell(1);
    
    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }
    
    // Return value
    return ZombieIsVIP(iD);
}

/**
 * Gets the skill duration of the zombie class.
 *
 * native float ZP_GetZombieClassSkillDuration(iD);
 **/
public int API_GetZombieClassSkillDuration(Handle isPlugin, int iNumParams)
{
    // Gets class index from native cell
    int iD = GetNativeCell(1);
    
    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }
    
    // Return value (Float fix)
    return view_as<int>(ZombieGetSkillDuration(iD));
}

/**
 * Gets the skill countdown of the zombie class.
 *
 * native float ZP_GetZombieClassSkillCountdown(iD);
 **/
public int API_GetZombieClassSkillCountdown(Handle isPlugin, int iNumParams)
{
    // Gets class index from native cell
    int iD = GetNativeCell(1);
    
    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }
    
    // Return value (Float fix)
    return view_as<int>(ZombieGetSkillCountDown(iD));
}

/**
 * Gets the regen of the zombie class.
 *
 * native int ZP_GetZombieClassRegen(iD);
 **/
public int API_GetZombieClassRegen(Handle isPlugin, int iNumParams)
{
    // Gets class index from native cell
    int iD = GetNativeCell(1);
    
    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }
    
    // Return value
    return ZombieGetRegenHealth(iD);
}

/**
 * Gets the regen interval of the zombie class.
 *
 * native float ZP_GetZombieClassRegenInterval(iD);
 **/
public int API_GetZombieClassRegenInterval(Handle isPlugin, int iNumParams)
{
    // Gets class index from native cell
    int iD = GetNativeCell(1);
    
    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }
    
    // Return the value (Float fix)
    return view_as<int>(ZombieGetRegenInterval(iD));
}

/**
 * Gets the index of the zombie class claw model.
 *
 * native int ZP_GetZombieClassClawID(iD);
 **/
public int API_GetZombieClassClawID(Handle isPlugin, int iNumParams)
{
    // Gets class index from native cell
    int iD = GetNativeCell(1);
    
    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }
    
    // Return value
    return ZombieGetClawIndex(iD);
}

/**
 * Gets the index of the zombie class grenade model.
 *
 * native int ZP_GetZombieClassGrenadeID(iD);
 **/
public int API_GetZombieClassGrenadeID(Handle isPlugin, int iNumParams)
{
    // Gets class index from native cell
    int iD = GetNativeCell(1);
    
    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }
    
    // Return value
    return ZombieGetGrenadeIndex(iD);
}

/**
 * Print the info about the zombie class.
 *
 * native void ZP_PrintZombieClassInfo(clientIndex, iD);
 **/
public int API_PrintZombieClassInfo(Handle isPlugin, int iNumParams)
{
    // If help messages disable, then stop 
    if(!gCvarList[CVAR_MESSAGES_HELP].BoolValue)
    {
        return -1;
    }
    
    // Gets real player index from native cell 
    int clientIndex = GetNativeCell(1);
    
    // Validate client
    if(!IsPlayerExist(clientIndex, false))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Player doens't exist (%d)", clientIndex);
        return -1;
    }
    
    // Gets class index from native cell
    int iD = GetNativeCell(2);
    
    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }

    // Gets zombie name
    static char sZombieName[SMALL_LINE_LENGTH];
    ZombieGetName(iD, sZombieName, sizeof(sZombieName));

    // If help messages enabled, show info
    TranslationPrintToChat(clientIndex, "Zombie info", sZombieName, ZombieGetHealth(iD), ZombieGetSpeed(iD), ZombieGetGravity(iD));
    
    // Return on success
    return sizeof(sZombieName);
}

/**
 * Gets the death sound of a zombie class at a given index.
 *
 * native void ZP_GetZombieClassSoundDeath(iD, sSound, maxLen);
 **/
public int API_GetZombieClassSoundDeath(Handle isPlugin, int iNumParams)
{
    // Gets class index from native cell
    int iD = GetNativeCell(1);

    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }
    
    // Gets string size from native cell
    int maxLen = GetNativeCell(3);

    // Validate size
    if(!maxLen)
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "No buffer size");
        return -1;
    }
    
    // Initialize sound char
    static char sSound[SMALL_LINE_LENGTH];
    ZombieGetSoundDeath(iD, sSound, sizeof(sSound));

    // Return on success
    return SetNativeString(2, sSound, maxLen);
}

/**
 * Gets the hurt sound of a zombie class at a given index.
 *
 * native void ZP_GetZombieClassSoundHurt(iD, sSound, maxLen);
 **/
public int API_GetZombieClassSoundHurt(Handle isPlugin, int iNumParams)
{
    // Gets class index from native cell
    int iD = GetNativeCell(1);

    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }
    
    // Gets string size from native cell
    int maxLen = GetNativeCell(3);

    // Validate size
    if(!maxLen)
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "No buffer size");
        return -1;
    }
    
    // Initialize sound char
    static char sSound[SMALL_LINE_LENGTH];
    ZombieGetSoundHurt(iD, sSound, sizeof(sSound));

    // Return on success
    return SetNativeString(2, sSound, maxLen);
}

/**
 * Gets the idle sound of a zombie class at a given index.
 *
 * native void ZP_GetZombieClassSoundIdle(iD, sSound, maxLen);
 **/
public int API_GetZombieClassSoundIdle(Handle isPlugin, int iNumParams)
{
    // Gets class index from native cell
    int iD = GetNativeCell(1);

    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }
    
    // Gets string size from native cell
    int maxLen = GetNativeCell(3);

    // Validate size
    if(!maxLen)
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "No buffer size");
        return -1;
    }
    
    // Initialize sound char
    static char sSound[SMALL_LINE_LENGTH];
    ZombieGetSoundIdle(iD, sSound, sizeof(sSound));

    // Return on success
    return SetNativeString(2, sSound, maxLen);
}

/**
 * Gets the respawn sound of a zombie class at a given index.
 *
 * native void ZP_GetZombieClassSoundRespawn(iD, sSound, maxLen);
 **/
public int API_GetZombieClassSoundRespawn(Handle isPlugin, int iNumParams)
{
    // Gets class index from native cell
    int iD = GetNativeCell(1);

    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }
    
    // Gets string size from native cell
    int maxLen = GetNativeCell(3);

    // Validate size
    if(!maxLen)
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "No buffer size");
        return -1;
    }
    
    // Initialize sound char
    static char sSound[SMALL_LINE_LENGTH];
    ZombieGetSoundRespawn(iD, sSound, sizeof(sSound));

    // Return on success
    return SetNativeString(2, sSound, maxLen);
}

/**
 * Gets the burn sound of a zombie class at a given index.
 *
 * native void ZP_GetZombieClassSoundBurn(iD, sSound, maxLen);
 **/
public int API_GetZombieClassSoundBurn(Handle isPlugin, int iNumParams)
{
    // Gets class index from native cell
    int iD = GetNativeCell(1);

    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }
    
    // Gets string size from native cell
    int maxLen = GetNativeCell(3);

    // Validate size
    if(!maxLen)
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "No buffer size");
        return -1;
    }
    
    // Initialize sound char
    static char sSound[SMALL_LINE_LENGTH];
    ZombieGetSoundBurn(iD, sSound, sizeof(sSound));

    // Return on success
    return SetNativeString(2, sSound, maxLen);
}

/**
 * Gets the attack sound of a zombie class at a given index.
 *
 * native void ZP_GetZombieClassSoundAttack(iD, sSound, maxLen);
 **/
public int API_GetZombieClassSoundAttack(Handle isPlugin, int iNumParams)
{
    // Gets class index from native cell
    int iD = GetNativeCell(1);

    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }
    
    // Gets string size from native cell
    int maxLen = GetNativeCell(3);

    // Validate size
    if(!maxLen)
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "No buffer size");
        return -1;
    }
    
    // Initialize sound char
    static char sSound[SMALL_LINE_LENGTH];
    ZombieGetSoundAttack(iD, sSound, sizeof(sSound));

    // Return on success
    return SetNativeString(2, sSound, maxLen);
}

/**
 * Gets the footstep sound of a zombie class at a given index.
 *
 * native void ZP_GetZombieClassSoundFoot(iD, sSound, maxLen);
 **/
public int API_GetZombieClassSoundFoot(Handle isPlugin, int iNumParams)
{
    // Gets class index from native cell
    int iD = GetNativeCell(1);

    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }
    
    // Gets string size from native cell
    int maxLen = GetNativeCell(3);

    // Validate size
    if(!maxLen)
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "No buffer size");
        return -1;
    }
    
    // Initialize sound char
    static char sSound[SMALL_LINE_LENGTH];
    ZombieGetSoundFoot(iD, sSound, sizeof(sSound));

    // Return on success
    return SetNativeString(2, sSound, maxLen);
}

/**
 * Gets the regeneration sound of a zombie class at a given index.
 *
 * native void ZP_GetZombieClassSoundRegen(iD, sSound, maxLen);
 **/
public int API_GetZombieClassSoundRegen(Handle isPlugin, int iNumParams)
{
    // Gets class index from native cell
    int iD = GetNativeCell(1);

    // Validate index
    if(iD >= GetArraySize(arrayZombieClasses))
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "Invalid the class index (%d)", iD);
        return -1;
    }
    
    // Gets string size from native cell
    int maxLen = GetNativeCell(3);

    // Validate size
    if(!maxLen)
    {
        LogEvent(false, LogType_Native, LOG_CORE_EVENTS, LogModule_Zombieclasses, "Native Validation", "No buffer size");
        return -1;
    }
    
    // Initialize sound char
    static char sSound[SMALL_LINE_LENGTH];
    ZombieGetSoundRegen(iD, sSound, sizeof(sSound));

    // Return on success
    return SetNativeString(2, sSound, maxLen);
}

/*
 * Zombie classes data reading API.
 */

/**
 * Gets the name of a zombie class at a given index.
 *
 * @param iD                The class index.
 * @param sName             The string to return name in.
 * @param iMaxLen           The max length of the string.
 **/
stock void ZombieGetName(int iD, char[] sName, int iMaxLen)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Gets zombie class name
    arrayZombieClass.GetString(ZOMBIECLASSES_DATA_NAME, sName, iMaxLen);
}

/**
 * Gets the info of a zombie class at a given index.
 *
 * @param iD                The class index.
 * @param sInfo             The string to return info in.
 * @param iMaxLen           The max length of the string.
 **/
stock void ZombieGetInfo(int iD, char[] sInfo, int iMaxLen)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Gets zombie class info
    arrayZombieClass.GetString(ZOMBIECLASSES_DATA_INFO, sInfo, iMaxLen);
}

/**
 * Gets the player model of a zombie class at a given index.
 *
 * @param iD                The class index.
 * @param sModel            The string to return model in.
 * @param iMaxLen           The max length of the string.
 **/
stock void ZombieGetModel(int iD, char[] sModel, int iMaxLen)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Gets zombie class model
    arrayZombieClass.GetString(ZOMBIECLASSES_DATA_MODEL, sModel, iMaxLen);
}

/**
 * Gets the knife model of a zombie class at a given index.
 *
 * @param iD                The class index.
 * @param sModel            The string to return model in.
 * @param iMaxLen           The max length of the string.
 **/
stock void ZombieGetClawModel(int iD, char[] sModel, int iMaxLen)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Gets zombie class claw's model
    arrayZombieClass.GetString(ZOMBIECLASSES_DATA_CLAW, sModel, iMaxLen);
}

/**
 * Gets the grenade model of a zombie class at a given index.
 *
 * @param iD                The class index.
 * @param sModel            The string to return model in.
 * @param iMaxLen           The max length of the string.
 **/
stock void ZombieGetGrenadeModel(int iD, char[] sModel, int iMaxLen)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Gets zombie class grenade's model
    arrayZombieClass.GetString(ZOMBIECLASSES_DATA_GRENADE, sModel, iMaxLen);
}

/**
 * Gets the health of the zombie class.
 *
 * @param iD                The class index.
 * @return                  The health amount.    
 **/
stock int ZombieGetHealth(int iD)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Gets zombie class health
    return arrayZombieClass.Get(ZOMBIECLASSES_DATA_HEALTH);
}

/**
 * Gets the speed of the zombie class.
 *
 * @param iD                The class index.
 * @return                  The speed amount.    
 **/
stock float ZombieGetSpeed(int iD)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Gets zombie class speed 
    return arrayZombieClass.Get(ZOMBIECLASSES_DATA_SPEED);
}

/**
 * Gets the gravity of the zombie class.
 *
 * @param iD                The class index.
 * @return                  The gravity amount.    
 **/
stock float ZombieGetGravity(int iD)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Gets zombie class speed 
    return arrayZombieClass.Get(ZOMBIECLASSES_DATA_GRAVITY);
}

/**
 * Gets the knockback of the zombie class.
 *
 * @param iD                The class index.
 * @return                  The knockback amount.    
 **/
stock float ZombieGetKnockBack(int iD)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Gets zombie class knockback 
    return arrayZombieClass.Get(ZOMBIECLASSES_DATA_KNOCKBACK);
}

/**
 * Gets the level of the zombie class.
 *
 * @param iD                The class index.
 * @return                  The duration amount.    
 **/
stock int ZombieGetLevel(int iD)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Gets zombie class level 
    return arrayZombieClass.Get(ZOMBIECLASSES_DATA_LEVEL);
}

/**
 * Check the access to the zombie class.
 *
 * @param iD                The class index.
 * @return                  True or false.
 **/
stock bool ZombieIsVIP(int iD)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Gets zombie class access 
    return arrayZombieClass.Get(ZOMBIECLASSES_DATA_VIP);
}

/**
 * Gets the skill duration of the zombie class.
 *
 * @param iD                The class index.
 * @return                  The duration amount.    
 **/
stock float ZombieGetSkillDuration(int iD)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Gets zombie class skill's duration 
    return arrayZombieClass.Get(ZOMBIECLASSES_DATA_DURATION);
}

/**
 * Gets the skill countdown of the zombie class.
 *
 * @param iD                The class index.
 * @return                  The countdown amount.    
 **/
stock float ZombieGetSkillCountDown(int iD)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Gets zombie class skill's countdown  
    return arrayZombieClass.Get(ZOMBIECLASSES_DATA_COUNTDOWN);
}

/**
 * Gets the regen amount of the zombie class.
 *
 * @param iD                The class index.
 * @return                  The health amount.    
 **/
stock int ZombieGetRegenHealth(int iD)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Gets zombie class regen's health
    return arrayZombieClass.Get(ZOMBIECLASSES_DATA_REGENHEALTH);
}

/**
 * Gets the regen interval of the zombie class.
 *
 * @param iD                The class index.
 * @return                  The interval amount.    
 **/
stock float ZombieGetRegenInterval(int iD)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Gets zombie class regen's interval
    return arrayZombieClass.Get(ZOMBIECLASSES_DATA_REGENINTERVAL);
}

/**
 * Gets the index of the zombie class claw model.
 *
 * @param iD                The class index.
 * @return                  The model index.    
 **/
stock int ZombieGetClawIndex(int iD)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Gets zombie class claw's model index
    return arrayZombieClass.Get(ZOMBIECLASSES_DATA_CLAW_ID);
}

/**
 * Sets the index of the zombie class claw model.
 *
 * @param iD                The class index.
 * @param modelIndex        The model index.    
 **/
stock void ZombieSetClawIndex(int iD, int modelIndex)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Sets knife model index
    arrayZombieClass.Set(ZOMBIECLASSES_DATA_CLAW_ID, modelIndex);
}

/**
 * Gets the index of the zombie class grenade model.
 *
 * @param iD                The class index.
 * @return                  The model index.    
 **/
stock int ZombieGetGrenadeIndex(int iD)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Gets zombie class grenade's model index
    return arrayZombieClass.Get(ZOMBIECLASSES_DATA_GRENADE_ID);
}

/**
 * Sets the index of the zombie class grenade model.
 *
 * @param iD                The class index.
 * @param modelIndex        The model index.    
 **/
stock void ZombieSetGrenadeIndex(int iD, int modelIndex)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Sets knife model index
    arrayZombieClass.Set(ZOMBIECLASSES_DATA_GRENADE_ID, modelIndex);
}

/**
 * Gets the death sound of a zombie class at a given index.
 *
 * @param iD                The class index.
 * @param sSound            The string to return sound in.
 * @param iMaxLen           The max length of the string.
 **/
stock void ZombieGetSoundDeath(int iD, char[] sSound, int iMaxLen)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Gets zombie class death sound
    arrayZombieClass.GetString(ZOMBIECLASSES_DATA_SOUNDDEATH, sSound, iMaxLen);
}

/**
 * Gets the hurt sound of a zombie class at a given index.
 *
 * @param iD                The class index.
 * @param sSound            The string to return sound in.
 * @param iMaxLen           The max length of the string.
 **/
stock void ZombieGetSoundHurt(int iD, char[] sSound, int iMaxLen)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Gets zombie class hurt sound
    arrayZombieClass.GetString(ZOMBIECLASSES_DATA_SOUNDHURT, sSound, iMaxLen);
}

/**
 * Gets the idle sound of a zombie class at a given index.
 *
 * @param iD                The class index.
 * @param sSound            The string to return sound in.
 * @param iMaxLen           The max length of the string.
 **/
stock void ZombieGetSoundIdle(int iD, char[] sSound, int iMaxLen)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Gets zombie class idle sound
    arrayZombieClass.GetString(ZOMBIECLASSES_DATA_SOUNDIDLE, sSound, iMaxLen);
}

/**
 * Gets the respawn sound of a zombie class at a given index.
 *
 * @param iD                The class index.
 * @param sSound            The string to return sound in.
 * @param iMaxLen           The max length of the string.
 **/
stock void ZombieGetSoundRespawn(int iD, char[] sSound, int iMaxLen)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Gets zombie class respawn sound
    arrayZombieClass.GetString(ZOMBIECLASSES_DATA_SOUNDRESPAWN, sSound, iMaxLen);
}

/**
 * Gets the burn sound of a zombie class at a given index.
 *
 * @param iD                The class index.
 * @param sSound            The string to return sound in.
 * @param iMaxLen           The max length of the string.
 **/
stock void ZombieGetSoundBurn(int iD, char[] sSound, int iMaxLen)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Gets zombie class idle sound
    arrayZombieClass.GetString(ZOMBIECLASSES_DATA_SOUNDBURN, sSound, iMaxLen);
}

/**
 * Gets the attack sound of a zombie class at a given index.
 *
 * @param iD                The class index.
 * @param sSound            The string to return sound in.
 * @param iMaxLen           The max length of the string.
 **/
stock void ZombieGetSoundAttack(int iD, char[] sSound, int iMaxLen)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Gets zombie class idle sound
    arrayZombieClass.GetString(ZOMBIECLASSES_DATA_SOUNDATTACK, sSound, iMaxLen);
}

/**
 * Gets the footstep sound of a zombie class at a given index.
 *
 * @param iD                The class index.
 * @param sSound            The string to return sound in.
 * @param iMaxLen           The max length of the string.
 **/
stock void ZombieGetSoundFoot(int iD, char[] sSound, int iMaxLen)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Gets zombie class footstep sound
    arrayZombieClass.GetString(ZOMBIECLASSES_DATA_SOUNDFOOTSTEP, sSound, iMaxLen);
}

/**
 * Gets the regeneration sound of a zombie class at a given index.
 *
 * @param iD                The class index.
 * @param sSound            The string to return sound in.
 * @param iMaxLen           The max length of the string.
 **/
stock void ZombieGetSoundRegen(int iD, char[] sSound, int iMaxLen)
{
    // Gets array handle of zombie class at given index
    ArrayList arrayZombieClass = arrayZombieClasses.Get(iD);

    // Gets zombie class regeneration sound
    arrayZombieClass.GetString(ZOMBIECLASSES_DATA_SOUNDREGEN, sSound, iMaxLen);
}

/*
 * Stocks zombie classes API.
 */

/**
 * Validate zombie class for client's availability.
 *
 * @param clientIndex       The client index.
 **/
void ZombieOnValidate(int clientIndex)
{
    // Gets array size
    int iSize = GetArraySize(arrayZombieClasses);
    
    // Gets client's access
    bool IsVIP = IsPlayerHasFlag(clientIndex, Admin_Custom1);
    
    // Choose random zombie class for the client
    if(IsFakeClient(clientIndex) || (!IsVIP && gCvarList[CVAR_ZOMBIE_RANDOM_CLASS].BoolValue) || iSize <= gClientData[clientIndex][Client_ZombieClass])
    {
        gClientData[clientIndex][Client_ZombieClass] = GetRandomInt(0, iSize-1);
    }
    
    // Validate that user does not have VIP flag to play it
    if(!IsVIP)
    {
        // But have privileged zombie class by default
        if(ZombieIsVIP(gClientData[clientIndex][Client_ZombieClass]))
        {
            // Choose any accessable zombie class
            for(int i = 0; i < iSize; i++)
            {
                // Skip all non-accessable zombie classes
                if(ZombieIsVIP(i))
                {
                    continue;
                }
                
                // Update zombie class
                gClientData[clientIndex][Client_ZombieClassNext]  = i;
                gClientData[clientIndex][Client_ZombieClass] = i;
            }
        }
    }
    
    // Validate that user does not have level to play it
    if(ZombieGetLevel(gClientData[clientIndex][Client_ZombieClass]) > gClientData[clientIndex][Client_Level])
    {
        // Choose any accessable zombie class
        for(int i = 0; i < iSize; i++)
        {
            // Skip all non-accessable zombie classes
            if(ZombieGetLevel(i) > gClientData[clientIndex][Client_Level])
            {
                continue;
            }
            
            // Update zombie class
            gClientData[clientIndex][Client_ZombieClassNext]  = i;
            gClientData[clientIndex][Client_ZombieClass] = i;
        }
    }
}

/**
 * Create the zombie class menu.
 *
 * @param clientIndex       The client index.
 **/
void ZombieMenu(int clientIndex) 
{
    // Validate client
    if(!IsPlayerExist(clientIndex, false))
    {
        return;
    }

    // Initialize chars
    static char sBuffer[NORMAL_LINE_LENGTH];
    static char sName[SMALL_LINE_LENGTH];
    static char sInfo[SMALL_LINE_LENGTH];
    static char sLevel[SMALL_LINE_LENGTH];

    // Create menu handle
    Menu hMenu = CreateMenu(ZombieMenuSlots);

    // Sets the language to target
    SetGlobalTransTarget(clientIndex);
    
    // Sets title
    hMenu.SetTitle("%t", "Choose zombieclass");
    
    // i = Zombie class number
    int iCount = GetArraySize(arrayZombieClasses);
    for(int i = 0; i < iCount; i++)
    {
        // Gets zombie class name
        ZombieGetName(i, sName, sizeof(sName));
        if(!IsCharUpper(sName[0]) && !IsCharNumeric(sName[0])) sName[0] = CharToUpper(sName[0]);
        
        // Format some chars for showing in menu
        Format(sLevel, sizeof(sLevel), "[LVL:%d]", ZombieGetLevel(i));
        Format(sBuffer, sizeof(sBuffer), "%t    %s", sName, ZombieIsVIP(i) ? "[VIP]" : ZombieGetLevel(i) > 1 ? sLevel : "");
        
        // Show option
        IntToString(i, sInfo, sizeof(sInfo));
        hMenu.AddItem(sInfo, sBuffer, MenuGetItemDraw(((!IsPlayerHasFlag(clientIndex, Admin_Custom1) && ZombieIsVIP(i)) || gClientData[clientIndex][Client_Level] < ZombieGetLevel(i) || gClientData[clientIndex][Client_ZombieClassNext] == i) ? false : true));
    }

    // Sets exit and back button
    hMenu.ExitBackButton = true;

    // Sets options and display it
    hMenu.OptionFlags = MENUFLAG_BUTTON_EXIT|MENUFLAG_BUTTON_EXITBACK;
    hMenu.Display(clientIndex, MENU_TIME_FOREVER); 
}

/**
 * Called when client selects option in the zombie class menu, and handles it.
 *  
 * @param hMenu             The handle of the menu being used.
 * @param mAction           The action done on the menu (see menus.inc, enum MenuAction).
 * @param clientIndex       The client index.
 * @param mSlot             The slot index selected (starting from 0).
 **/ 
public int ZombieMenuSlots(Menu hMenu, MenuAction mAction, int clientIndex, int mSlot)
{
    // Switch the menu action
    switch(mAction)
    {
        // Client hit 'Exit' button
        case MenuAction_End :
        {
            delete hMenu;
        }
        
        // Client hit 'Back' button
        case MenuAction_Cancel :
        {
            if(mSlot == MenuCancel_ExitBack)
            {
                // Open main menu back
                MenuMain(clientIndex);
            }
        }
        
        // Client selected an option
        case MenuAction_Select :
        {
            // Validate client
            if(!IsPlayerExist(clientIndex, false))
            {
                return;
            }

            // Initialize char
            static char sInfo[SMALL_LINE_LENGTH];

            // Gets ID of zombie class
            hMenu.GetItem(mSlot, sInfo, sizeof(sInfo));
            int iD = StringToInt(sInfo);
            
            // Sets next zombie class
            gClientData[clientIndex][Client_ZombieClassNext] = iD;
    
            // Gets zombie name
            ZombieGetName(iD, sInfo, sizeof(sInfo));

            // If help messages enabled, show info
            if(gCvarList[CVAR_MESSAGES_HELP].BoolValue) TranslationPrintToChat(clientIndex, "Zombie info", sInfo, ZombieGetHealth(iD), ZombieGetSpeed(iD), ZombieGetGravity(iD));
        }
    }
}
