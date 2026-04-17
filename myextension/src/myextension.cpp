// myextension.cpp
// Extension lib defines
#define LIB_NAME "MyExtension"
#define MODULE_NAME "myextension"

// include the Defold SDK
#include <dmsdk/sdk.h>

// generated from chat_ddf.proto
#include "chat_ddf.h"

// Queue of ChatMessage instances
static dmArray<dmChatDDF::ChatMessage*> g_ChatMessageQueue;


static int Reverse(lua_State* L)
{
    // The number of expected items to be on the Lua stack
    // once this struct goes out of scope
    DM_LUA_STACK_CHECK(L, 1);

    // Check and get parameter string from stack
    char* str = (char*)luaL_checkstring(L, 1);

    // Reverse the string
    int len = strlen(str);
    for(int i = 0; i < len / 2; i++) {
        const char a = str[i];
        const char b = str[len - i - 1];
        str[i] = b;
        str[len - i - 1] = a;
    }

    // Put the reverse string on the stack
    lua_pushstring(L, str);

    // Return 1 item
    return 1;
}

// The two functions below show how to work with protobuf messages.
//
// They show a fictional example of a chat application where messages can be
// sent, queued and later checked. The example doesn't actually send the
// messages anywhere, it simply puts the message in a queue and sends a
// notification to the calling script. The calling script can then get the
// message as a Lua table
// Note: The DDF acronym stands for Defold Data Format, but is in fact protobuf

// SendMessage
// This function will "send" a message by allocating a dmChatDDF::ChatMessage
// instance and a copy of the message text and put it in a queue.
// The function will also post a dmChatDDF::OnMessage to the calling script
// using dmGameObject::PostDDF() to notify the script that there are waiting
// messages
static int SendMessage(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);

    // allocate dmChatDDF::ChatMessage instance
    dmChatDDF::ChatMessage* chat_message = (dmChatDDF::ChatMessage*)malloc(sizeof(dmChatDDF::ChatMessage));
    chat_message->m_From = dmScript::CheckHash(L, 1);
    chat_message->m_To = dmScript::CheckHash(L, 2);
    // copy message text
    size_t text_size;
    const char* text = luaL_checklstring(L, 3, &text_size);
    char* text_copy = (char*)malloc(text_size + 1);
    dmStrlCpy(text_copy, text, text_size + 1);
    chat_message->m_Text = text_copy;

    // push chat message to queue
    if (g_ChatMessageQueue.Full())
    {
        g_ChatMessageQueue.OffsetCapacity(1);
    }
    g_ChatMessageQueue.Push(chat_message);

    // construct a sender and receiver URL for use when posting the message
    // the sender and receiver will be the URL of the calling game object
    dmMessage::URL sender;
    dmMessage::URL receiver;
    dmGameObject::HInstance caller_instance = dmScript::CheckGOInstance(L);
    dmGameObject::HCollection collection = dmGameObject::GetCollection(caller_instance);
    sender.m_Path = dmGameObject::GetIdentifier(caller_instance);
    sender.m_Socket = dmGameObject::GetMessageSocket(collection);
    receiver.m_Path = dmGameObject::GetIdentifier(caller_instance);
    receiver.m_Socket = dmGameObject::GetMessageSocket(collection);

    // post notification to receiver
    dmChatDDF::OnMessage on_message;
    dmGameObject::Result result = dmGameObject::PostDDF(&on_message, &sender, &receiver, 0, false);

    return 0;
}

// This function will take the most recently added message from the queue and
// return it as a Lua table. Conversion is done using dmScript::PushDDF()
// The message is freed when it is taken from the queue.
static int NextMessage(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 1);

    if (g_ChatMessageQueue.Empty())
    {
        lua_pushnil(L);
        return 1;
    }

    dmChatDDF::ChatMessage* chat_message = g_ChatMessageQueue.Back();
    dmScript::PushDDF(L, dmChatDDF::ChatMessage::m_DDFDescriptor, (const char*)chat_message, false);
    free((void*)chat_message->m_Text);
    free((void*)chat_message);
    g_ChatMessageQueue.Pop();

    return 1;
}

// Functions exposed to Lua
static const luaL_reg Module_methods[] =
{
    {"reverse", Reverse},
    {"send_message", SendMessage},
    {"next_message", NextMessage},
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

static dmExtension::Result AppInitializeMyExtension(dmExtension::AppParams* params)
{
    dmLogInfo("AppInitializeMyExtension");
    return dmExtension::RESULT_OK;
}

static dmExtension::Result InitializeMyExtension(dmExtension::Params* params)
{
    // Init Lua
    LuaInit(params->m_L);
    dmLogInfo("Registered %s Extension", MODULE_NAME);
    return dmExtension::RESULT_OK;
}

static dmExtension::Result AppFinalizeMyExtension(dmExtension::AppParams* params)
{
    dmLogInfo("AppFinalizeMyExtension");

    // Remove and free any queued messages
    while (!g_ChatMessageQueue.Empty())
    {
        dmChatDDF::ChatMessage* chat_message = g_ChatMessageQueue.Back();
        free((void*)chat_message->m_Text);
        free((void*)chat_message);
        g_ChatMessageQueue.Pop();
    }

    return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizeMyExtension(dmExtension::Params* params)
{
    dmLogInfo("FinalizeMyExtension");
    return dmExtension::RESULT_OK;
}

static dmExtension::Result OnUpdateMyExtension(dmExtension::Params* params)
{
    dmLogInfo("OnUpdateMyExtension");
    return dmExtension::RESULT_OK;
}

static void OnEventMyExtension(dmExtension::Params* params, const dmExtension::Event* event)
{
    switch(event->m_Event)
    {
        case dmExtension::EVENT_ID_ACTIVATEAPP:
            dmLogInfo("OnEventMyExtension - EVENT_ID_ACTIVATEAPP");
            break;
        case dmExtension::EVENT_ID_DEACTIVATEAPP:
            dmLogInfo("OnEventMyExtension - EVENT_ID_DEACTIVATEAPP");
            break;
        case dmExtension::EVENT_ID_ICONIFYAPP:
            dmLogInfo("OnEventMyExtension - EVENT_ID_ICONIFYAPP");
            break;
        case dmExtension::EVENT_ID_DEICONIFYAPP:
            dmLogInfo("OnEventMyExtension - EVENT_ID_DEICONIFYAPP");
            break;
        case dmExtension::EVENT_ID_ENGINE_INITIALIZED:
            dmLogInfo("OnEventMyExtension - EVENT_ID_ENGINE_INITIALIZED");
            break;
        case dmExtension::EVENT_ID_ENGINE_DELETE:
            dmLogInfo("OnEventMyExtension - EVENT_ID_ENGINE_DELETE");
            break;
        default:
            dmLogWarning("OnEventMyExtension - Unknown event id");
            break;
    }
}

// Defold SDK uses a macro for setting up extension entry points:
//
// DM_DECLARE_EXTENSION(symbol, name, app_init, app_final, init, update, on_event, final)

// MyExtension is the C++ symbol that holds all relevant extension data.
// It must match the name field in the `ext.manifest`
DM_DECLARE_EXTENSION(MyExtension, LIB_NAME, AppInitializeMyExtension, AppFinalizeMyExtension, InitializeMyExtension, OnUpdateMyExtension, OnEventMyExtension, FinalizeMyExtension)
