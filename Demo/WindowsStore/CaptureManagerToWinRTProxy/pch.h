//
// pch.h
// Header for standard system include files.
//

#pragma once

#include <collection.h>
#include <ppltasks.h>
#include <memory>
#include <Robuffer.h>

#define IID_PPV_IUNKNOWN_ARGS(ppType) __uuidof(**(ppType)), (IUnknown**)(ppType)

#define CHK(Func) if(FAILED(Func)) break