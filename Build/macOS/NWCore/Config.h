//
//  Config.h
//  NWCore
//
//  Created by User on 2017/9/1.
//  Copyright © 2017年 Infinideastudio. All rights reserved.
//

#pragma once

#if defined(__cplusplus)
#define FOUNDATION_EXTERN extern "C"
#else
#define FOUNDATION_EXTERN extern
#endif

#if TARGET_OS_WIN32

#if defined(NSBUILDINGFOUNDATION)
#define FOUNDATION_EXPORT FOUNDATION_EXTERN __declspec(dllexport)
#else
#define FOUNDATION_EXPORT FOUNDATION_EXTERN __declspec(dllimport)
#endif

#define FOUNDATION_IMPORT FOUNDATION_EXTERN __declspec(dllimport)

#else
#define FOUNDATION_EXPORT  FOUNDATION_EXTERN
#define FOUNDATION_IMPORT FOUNDATION_EXTERN
#endif

#define NWCORE_API FOUNDATION_EXPORT
