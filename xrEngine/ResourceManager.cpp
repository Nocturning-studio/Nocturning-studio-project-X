// TextureManager.cpp: implementation of the CResourceManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#pragma warning(disable : 4995)
#include <d3dx9.h>
#pragma warning(default : 4995)

#include "ResourceManager.h"
#include "blenders\blender.h"
#include "blenders\blender_recorder.h"
#include "tss.h"

void fix_texture_name(LPSTR fn)
{
    LPSTR _ext = strext(fn);
    if (_ext && (0 == stricmp(_ext, ".tga") || 0 == stricmp(_ext, ".dds") || 0 == stricmp(_ext, ".bmp") ||
                 0 == stricmp(_ext, ".ogm")))
        *_ext = 0;
}
//--------------------------------------------------------------------------------------------------------------
template <class T> BOOL reclaim(xr_vector<T *> &vec, const T *ptr)
{
    xr_vector<T *>::iterator it = vec.begin();
    xr_vector<T *>::iterator end = vec.end();
    for (; it != end; it++)
        if (*it == ptr)
        {
            vec.erase(it);
            return TRUE;
        }
    return FALSE;
}

//--------------------------------------------------------------------------------------------------------------
IBlender *CResourceManager::_GetBlender(LPCSTR Name)
{
    R_ASSERT(Name && Name[0]);

    LPSTR N = LPSTR(Name);
    map_Blender::iterator I = m_blenders.find(N);
#ifdef _EDITOR
    if (I == m_blenders.end())
        return 0;
#else
    if (I == m_blenders.end())
    {
        Debug.fatal(DEBUG_INFO, "Shader '%s' not found in library.", Name);
        return 0;
    }
#endif
    else
        return I->second;
}

IBlender *CResourceManager::_FindBlender(LPCSTR Name)
{
    if (!(Name && Name[0]))
        return 0;

    LPSTR N = LPSTR(Name);
    map_Blender::iterator I = m_blenders.find(N);
    if (I == m_blenders.end())
        return 0;
    else
        return I->second;
}

void CResourceManager::ED_UpdateBlender(LPCSTR Name, IBlender *data)
{
    LPSTR N = LPSTR(Name);
    map_Blender::iterator I = m_blenders.find(N);
    if (I != m_blenders.end())
    {
        R_ASSERT(data->getDescription().CLS == I->second->getDescription().CLS);
        xr_delete(I->second);
        I->second = data;
    }
    else
    {
        m_blenders.insert(mk_pair(xr_strdup(Name), data));
    }
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CResourceManager::_ParseList(sh_list &dest, LPCSTR names)
{
    if (0 == names)
        names = "$null";

    ZeroMemory(&dest, sizeof(dest));
    char *P = (char *)names;
    svector<char, 128> N;

    while (*P)
    {
        if (*P == ',')
        {
            // flush
            N.push_back(0);
            strlwr(N.begin());

            fix_texture_name(N.begin());
            //. andy			if (strext(N.begin())) *strext(N.begin())=0;
            dest.push_back(N.begin());
            N.clear();
        }
        else
        {
            N.push_back(*P);
        }
        P++;
    }
    if (N.size())
    {
        // flush
        N.push_back(0);
        strlwr(N.begin());

        fix_texture_name(N.begin());
        //. andy		if (strext(N.begin())) *strext(N.begin())=0;
        dest.push_back(N.begin());
    }
}

ShaderElement *CResourceManager::_CreateElement(ShaderElement &S)
{
    if (S.passes.empty())
        return 0;

    // Search equal in shaders array
    for (u32 it = 0; it < v_elements.size(); it++)
        if (S.equal(*(v_elements[it])))
            return v_elements[it];

    // Create _new_ entry
    ShaderElement *N = xr_new<ShaderElement>(S);
    N->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    v_elements.push_back(N);
    return N;
}

void CResourceManager::_DeleteElement(const ShaderElement *S)
{
    if (0 == (S->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;
    if (reclaim(v_elements, S))
        return;
    Msg("! ERROR: Failed to find compiled 'shader-element'");
}

Shader *CResourceManager::_cpp_Create(IBlender *B, LPCSTR s_shader, LPCSTR s_textures, LPCSTR s_constants,
                                      LPCSTR s_matrices)
{
    CBlender_Compile C;
    Shader S;

    //.
    // if (strstr(s_shader,"transparent"))	__asm int 3;

    // Access to template
    C.BT = B;
    C.bEditor = FALSE;
    C.bDetail = FALSE;
#ifdef _EDITOR
    if (!C.BT)
    {
        ELog.Msg(mtError, "Can't find shader '%s'", s_shader);
        return 0;
    }
    C.bEditor = TRUE;
#endif

    // Parse names
    _ParseList(C.L_textures, s_textures);
    _ParseList(C.L_constants, s_constants);
    _ParseList(C.L_matrices, s_matrices);

    // Compile element	(LOD0 - HQ)
    {
        C.iElement = 0;
        C.bDetail = m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);
        //.		C.bDetail			= _GetDetailTexture(*C.L_textures[0],C.detail_texture,C.detail_scaler);
        ShaderElement E;
        C._cpp_Compile(&E);
        S.E[0] = _CreateElement(E);
    }

    // Compile element	(LOD1)
    {
        C.iElement = 1;
        //.		C.bDetail			= _GetDetailTexture(*C.L_textures[0],C.detail_texture,C.detail_scaler);
        C.bDetail = m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);
        ShaderElement E;
        C._cpp_Compile(&E);
        S.E[1] = _CreateElement(E);
    }

    // Compile element
    {
        C.iElement = 2;
        C.bDetail = FALSE;
        ShaderElement E;
        C._cpp_Compile(&E);
        S.E[2] = _CreateElement(E);
    }

    // Compile element
    {
        C.iElement = 3;
        C.bDetail = FALSE;
        ShaderElement E;
        C._cpp_Compile(&E);
        S.E[3] = _CreateElement(E);
    }

    // Compile element
    {
        C.iElement = 4;
        C.bDetail = TRUE; //.$$$ HACK :)
        ShaderElement E;
        C._cpp_Compile(&E);
        S.E[4] = _CreateElement(E);
    }

    // Compile element
    {
        C.iElement = 5;
        C.bDetail = FALSE;
        ShaderElement E;
        C._cpp_Compile(&E);
        S.E[5] = _CreateElement(E);
    }

    // Search equal in shaders array
    for (u32 it = 0; it < v_shaders.size(); it++)
        if (S.equal(v_shaders[it]))
            return v_shaders[it];

    // Create _new_ entry
    Shader *N = xr_new<Shader>(S);
    N->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    v_shaders.push_back(N);
    return N;
}

Shader *CResourceManager::_cpp_Create(LPCSTR s_shader, LPCSTR s_textures, LPCSTR s_constants, LPCSTR s_matrices)
{
#ifndef DEDICATED_SERVER
    return _cpp_Create(_GetBlender(s_shader ? s_shader : "null"), s_shader, s_textures, s_constants, s_matrices);
#else
    return NULL;
#endif
}

Shader *CResourceManager::Create(IBlender *B, LPCSTR s_shader, LPCSTR s_textures, LPCSTR s_constants, LPCSTR s_matrices)
{
#ifndef DEDICATED_SERVER
    return _cpp_Create(B, s_shader, s_textures, s_constants, s_matrices);
#else
    return NULL;
#endif
}

Shader *CResourceManager::Create(LPCSTR s_shader, LPCSTR s_textures, LPCSTR s_constants, LPCSTR s_matrices)
{
#ifndef DEDICATED_SERVER
#ifndef _EDITOR
    if (_lua_HasShader(s_shader))
        return _lua_Create(s_shader, s_textures);
    else
#endif
        return _cpp_Create(s_shader, s_textures, s_constants, s_matrices);
#else
    return NULL;
#endif
}

void CResourceManager::Delete(const Shader *S)
{
    if (0 == (S->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;
    if (reclaim(v_shaders, S))
        return;
    Msg("! ERROR: Failed to find complete shader");
}

#define MT_TEXTURES

#ifdef MT_TEXTURES
// �������� ������� ����������� ��������.
// ������� Morrey �� ������� �������� � ���� �������,
//	Maks0 �� ��������� ��� ����������� ���������� ���� ���������� �
//	������������ ���������������� ����� ������� � VAX, ������� ������� �������

#include <thread>
xr_vector<CTexture *> tex_to_load;

void TextureLoading(u16 thread_num, u32 lowerbound, u32 upperbound)
{
    Msg("* THREAD #%d: Started.", thread_num);

    for (size_t i = lowerbound; i < upperbound; i++)
        tex_to_load[i]->Load();

    Msg("* THREAD #%d: Task Completed.", thread_num);
}

void CResourceManager::DeferredUpload()
{
    if (!Device.b_is_Ready)
        return;

    const u32 MinTexturesCntToUseMT = 100;

    tex_to_load.clear();
    Msg("* New phase started: Texture Loading, size = %d", m_textures.size());
    CTimer timer;
    timer.Start();

    if (m_textures.size() <= MinTexturesCntToUseMT || strstr(Core.Params, "-one_thread_load"))
    {
        Msg("* Phase info: Texture Loading -> Use one thread");

        for (map_TextureIt t = m_textures.begin(); t != m_textures.end(); t++)
            t->second->Load();
    }
    else
    {
        u32 th_count = std::thread::hardware_concurrency();
        u32 texCntOneThread = m_textures.size() / th_count;
        std::thread *th_arr = new std::thread[th_count];

        for (const auto &tex : m_textures)
            tex_to_load.push_back(tex.second);

        for (u16 i = 0; i < th_count; i++)
        {
            u32 from = i * texCntOneThread;
            u32 to = (i + 1) * texCntOneThread;

            if (i == th_count - 1)
                to = m_textures.size();

            th_arr[i] = std::thread(&TextureLoading, i, from, to);
        }

        for (size_t i = 0; i < th_count; i++)
            th_arr[i].join();

        delete[] th_arr;
        tex_to_load.clear();
    }

    Msg("* Phase time: %d ms", timer.GetElapsed_ms());
}
#else  // MT_TEXTURES
void CResourceManager::DeferredUpload()
{
    if (!Device.b_is_Ready)
        return;
    for (map_TextureIt t = m_textures.begin(); t != m_textures.end(); t++)
    {
        t->second->Load();
    }
}
#endif // MT_TEXTURES

void CResourceManager::DeferredUnload()
{
    if (!Device.b_is_Ready)
        return;
    for (map_TextureIt t = m_textures.begin(); t != m_textures.end(); t++)
        t->second->Unload();
}

#ifdef _EDITOR
void CResourceManager::ED_UpdateTextures(AStringVec *names)
{
    // 1. Unload
    if (names)
    {
        for (u32 nid = 0; nid < names->size(); nid++)
        {
            map_TextureIt I = m_textures.find((*names)[nid].c_str());
            if (I != m_textures.end())
                I->second->Unload();
        }
    }
    else
    {
        for (map_TextureIt t = m_textures.begin(); t != m_textures.end(); t++)
            t->second->Unload();
    }

    // 2. Load
    // DeferredUpload	();
}
#endif

void CResourceManager::_GetMemoryUsage(u32 &m_base, u32 &c_base, u32 &m_lmaps, u32 &c_lmaps)
{
    m_base = c_base = m_lmaps = c_lmaps = 0;

    map_Texture::iterator I = m_textures.begin();
    map_Texture::iterator E = m_textures.end();
    for (; I != E; I++)
    {
        u32 m = I->second->flags.MemoryUsage;
        if (strstr(I->first, "lmap"))
        {
            c_lmaps++;
            m_lmaps += m;
        }
        else
        {
            c_base++;
            m_base += m;
        }
    }
}
void CResourceManager::_DumpMemoryUsage()
{
    xr_multimap<u32, std::pair<u32, shared_str>> mtex;

    // sort
    {
        map_Texture::iterator I = m_textures.begin();
        map_Texture::iterator E = m_textures.end();
        for (; I != E; I++)
        {
            u32 m = I->second->flags.MemoryUsage;
            shared_str n = I->second->cName;
            mtex.insert(mk_pair(m, mk_pair(I->second->dwReference, n)));
        }
    }

    // dump
    {
        xr_multimap<u32, std::pair<u32, shared_str>>::iterator I = mtex.begin();
        xr_multimap<u32, std::pair<u32, shared_str>>::iterator E = mtex.end();
        for (; I != E; I++)
            Msg("* %4.1f : [%4d] %s", float(I->first) / 1024.f, I->second.first, I->second.second.c_str());
    }
}

void CResourceManager::Evict()
{
    CHK_DX(HW.pDevice->EvictManagedResources());
}
/*
BOOL	CResourceManager::_GetDetailTexture(LPCSTR Name,LPCSTR& T, R_constant_setup* &CS)
{
    LPSTR N = LPSTR(Name);
    map_TD::iterator I = m_td.find	(N);
    if (I!=m_td.end())
    {
        T	= I->second.T;
        CS	= I->second.cs;
        return TRUE;
    } else {
        return FALSE;
    }
}*/
