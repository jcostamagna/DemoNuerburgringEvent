#pragma once

#include "resource.h"
#include "Session.h"
#include "PostgreSQL.h"

void manageSession();

void stopSession();

void sessionRunningError();

void notSessionError();