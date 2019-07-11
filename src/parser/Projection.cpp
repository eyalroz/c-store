/* Copyright (c) 2005, Regents of Massachusetts Institute of Technology, 
 * Brandeis University, Brown University, and University of Massachusetts 
 * Boston. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *   - Redistributions in binary form must reproduce the above copyright 
 *     notice, this list of conditions and the following disclaimer in the 
 *     documentation and/or other materials provided with the distribution.
 *   - Neither the name of Massachusetts Institute of Technology, 
 *     Brandeis University, Brown University, or University of 
 *     Massachusetts Boston nor the names of its contributors may be used 
 *     to endorse or promote products derived from this software without 
 *     specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "Projection.h"

Projection::Projection()
{
  m_sName = "";
  m_sAlias = "";
}

Projection::Projection(string name)
{
  CatalogInstance* lpCatalog = NULL;
  //lpCatalog = new CatalogInstance();
  lpCatalog = CatalogInstance::getCatalog(); //new CatalogInstance();
  if (lpCatalog == NULL)
    parser_error("Unable to create Catalog.");
  else {
    if (!lpCatalog->isValidProjection(name))
      parser_error("pRojection " + name + " does not exist.");
  }
  m_sName = name;
  m_sAlias = "";
  //delete lpCatalog;
}

Projection::Projection(string name, string alias)
{
  m_sName = name;
  m_sAlias = alias;
}

Projection::~Projection()
{
	Log::writeToLog("Projection", 10, "~Projection");
}

void	Projection::set(string name)
{
  m_sName = name;  
}

void	Projection::setAlias(string alias)
{
  m_sAlias = alias;  
}

string	Projection::get()
{
  return m_sName;
}

string	Projection::getAlias()
{
  return m_sAlias;
}

string	Projection::toString()
{
	if( m_sAlias.empty() ) {
		return m_sName;
	} else {
		return m_sName + " " + m_sAlias;
	}
}

//---------------------------------------------------------------
// Memory management functions

// New a node and put it in the m_lpList
Projection* Projection::create()
{
	Log::writeToLog("Projection", 0, "Create");
		
	Projection* lpProjection = new Projection();
	pushBack(lpProjection);
	return lpProjection;
}

// New a node and put it in the m_lpList
Projection* Projection::create(string name)
{
	Log::writeToLog("Projection", 0, "Create");
		
	Projection* lpProjection = new Projection(name);
	pushBack(lpProjection);
	return lpProjection;
}

// New a node and put it in the m_lpList
Projection* Projection::create(string name, string alias)
{
	Log::writeToLog("Projection", 0, "Create");
		
	Projection* lpProjection = new Projection(name, alias);
	pushBack(lpProjection);
	return lpProjection;
}
		

