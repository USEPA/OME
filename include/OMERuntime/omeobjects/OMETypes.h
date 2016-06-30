//
//  OMETypes.h
//  OMERuntime
//
//  Created by Patrick  Wingo on 11/5/13.
//  Copyright (c) 2013 Patrick  Wingo. All rights reserved.
//

#pragma once
/** \defgroup omeTypes OME Type Flags
  @{
 */
/** @enum OMETYPE
 Enum for differentiating child types.
 Each child type should have a unique identifier as part of the OMETYPE enum
 */
enum OMETYPE {	OME_NULL=0,		OME_BASE,		OME_EVAL,		OME_STATEVAR,
				OME_VARIABLE,	OME_VARARRAY,	OME_TSVAR,		OME_FLOW,		
				OME_INFLUENCE,	OME_MODEL,		OME_SUBPORT,	OME_ASSOCPORT,
				OME_SPAWNER,	OME_LABEL,		OME_ITERCOND,	OME_SUBASSOC,
				OME_ALIAS,		OME_CONVEYOR,
    
				OMETYPE_COUNT
};
///@}
