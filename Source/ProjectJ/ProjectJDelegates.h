#pragma once

DECLARE_MULTICAST_DELEGATE_OneParam(FOnProjectJOneIntParamDelegate, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnProjectJOneBoolParamDelegate, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FProjectJOneNameDelegate, const FName&);