// myextension.cpp
// Extension lib defines
#define LIB_NAME "MyExtension"
#define MODULE_NAME "myextension"

// include the Defold SDK
#include <dmsdk/sdk.h>
#include <stdlib.h>

static int Rot13(lua_State* L)
{
    int top = lua_gettop(L);

    // Check and get parameter string from stack
    const char* str = luaL_checkstring(L, 1);

    // Allocate new string
    int len = strlen(str);
    char *rot = (char *) malloc(len + 1);

    // Iterate over the parameter string and create rot13 string
    for(int i = 0; i <= len; i++) {
        const char c = str[i];
        if((c >= 'A' && c <= 'M') || (c >= 'a' && c <= 'm')) {
            // Between A-M just add 13 to the char.
            rot[i] = c + 13;
        } else if((c >= 'N' && c <= 'Z') || (c >= 'n' && c <= 'z')) {
            // If rolling past 'Z' which happens below 'M', wrap back (subtract 13)
            rot[i] = c - 13;
        } else {
            // Leave character intact
            rot[i] = c;
        }
    }

    // Put the rotated string on the stack
    lua_pushstring(L, rot);

    // Free string memory. Lua has a copy by now.
    free(rot);

    // Assert that there is one item on the stack.
    assert(top + 1 == lua_gettop(L));

    // Return 1 item
    return 1;
}

// Functions exposed to Lua
static const luaL_reg Module_methods[] =
{
    {"rot13", Rot13},
    {0, 0}
};

static void LuaInit(lua_State* L)
{
    int top = lua_gettop(L);

    // Register lua names
    luaL_register(L, MODULE_NAME, Module_methods);

    lua_pop(L, 1);
    assert(top == lua_gettop(L));
}

dmExtension::Result AppInitializeMyExtension(dmExtension::AppParams* params)
{
    dmLogInfo("AppInitializeMyExtension\n");
    return dmExtension::RESULT_OK;
}

dmExtension::Result InitializeMyExtension(dmExtension::Params* params)
{
    // Init Lua
    LuaInit(params->m_L);
    dmLogInfo("Registered %s Extension\n", MODULE_NAME);
    return dmExtension::RESULT_OK;
}

dmExtension::Result AppFinalizeMyExtension(dmExtension::AppParams* params)
{
    dmLogInfo("AppFinalizeMyExtension\n");
    return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeMyExtension(dmExtension::Params* params)
{
    dmLogInfo("FinalizeMyExtension\n");
    return dmExtension::RESULT_OK;
}

dmExtension::Result OnUpdateMyExtension(dmExtension::Params* params)
{
    dmLogInfo("OnUpdateMyExtension\n");
    return dmExtension::RESULT_OK;
}

void OnEventMyExtension(dmExtension::Params* params, const dmExtension::Event* event)
{
    switch(event->m_Event)
    {
        case dmExtension::EVENT_ID_ACTIVATEAPP:
            dmLogInfo("OnEventMyExtension - EVENT_ID_ACTIVATEAPP\n");
            break;
        case dmExtension::EVENT_ID_DEACTIVATEAPP:
            dmLogInfo("OnEventMyExtension - EVENT_ID_DEACTIVATEAPP\n");
            break;
        case dmExtension::EVENT_ID_ICONIFYAPP:
            dmLogInfo("OnEventMyExtension - EVENT_ID_ICONIFYAPP\n");
            break;
        case dmExtension::EVENT_ID_DEICONIFYAPP:
            dmLogInfo("OnEventMyExtension - EVENT_ID_DEICONIFYAPP\n");
            break;
        default:
            dmLogWarning("OnEventMyExtension - Unknown event id\n");
            break;
    }
}

// Defold SDK uses a macro for setting up extension entry points:
//
// DM_DECLARE_EXTENSION(symbol, name, app_init, app_final, init, update, on_event, final)

// MyExtension is the C++ symbol that holds all relevant extension data.
// It must match the name field in the `ext.manifest`
DM_DECLARE_EXTENSION(MyExtension, LIB_NAME, AppInitializeMyExtension, AppFinalizeMyExtension, InitializeMyExtension, OnUpdateMyExtension, OnEventMyExtension, FinalizeMyExtension)
