/* Register names and numbers for SPARC DWARF.
   Copyright (C) 2005, 2006 Red Hat, Inc.
   This file is part of Red Hat elfutils.

   Red Hat elfutils is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by the
   Free Software Foundation; version 2 of the License.

   Red Hat elfutils is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with Red Hat elfutils; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301 USA.

   Red Hat elfutils is an included package of the Open Invention Network.
   An included package of the Open Invention Network is a package for which
   Open Invention Network licensees cross-license their patents.  No patent
   license is granted, either expressly or impliedly, by designation as an
   included package.  Should you wish to participate in the Open Invention
   Network licensing program, please visit www.openinventionnetwork.com
   <http://www.openinventionnetwork.com>.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <string.h>
#include <dwarf.h>

#define BACKEND sparc_
#include "libebl_CPU.h"

ssize_t
sparc_register_info (Ebl *ebl,
		     int regno, char *name, size_t namelen,
		     const char **prefix, const char **setname,
		     int *bits, int *type)
{
  const int nfp = 32 + (ebl->machine == EM_SPARC ? 0 : 16);
  const int nspec = ebl->machine == EM_SPARC ? 8 : 6;

  if (name == NULL)
    return 32 + nfp + nspec;

  if (regno < 0 || regno >= 32 + nfp + nspec || namelen < 6)
    return -1;

  *bits = ebl->machine == EM_SPARC ? 32 : 64;
  *type = DW_ATE_signed;

  *prefix = "%";

  if (regno >= 32 + nfp)
    {
      regno -= 32 + nfp;
      static const char names[2][8][6] =
	{
	  { "y", "psr", "wim", "tbr", "pc", "npc", "fsr", "csr" }, /* v8 */
	  { "pc", "npc", "state", "fsr", "fprs", "y" } /* v9 */
	};
      *setname = "control";
      *type = DW_ATE_unsigned;
      if ((ebl->machine != EM_SPARC ? 0 : 4) + 1 - (unsigned int) regno <= 1)
	*type = DW_ATE_address;
      return stpncpy (name, names[ebl->machine != EM_SPARC][regno],
		      namelen) + 1 - name;
    }

  if (regno < 32)
    {
      *setname = "integer";
      name[0] = "goli"[regno >> 3];
      name[1] = (regno & 7) + '0';
      namelen = 2;
      if ((regno & 8) && (regno & 7) == 6)
	*type = DW_ATE_address;
    }
  else
    {
      *setname = "FPU";
      *type = DW_ATE_float;

      regno -= 32;
      if (regno >= 32)
	regno = 32 + 2 * (regno - 32);
      else
	*bits = 32;

      name[0] = 'f';
      if (regno < 10)
	{
	  name[1] = regno + '0';
	  namelen = 2;
	}
      else
	{
	  name[1] = regno / 10 + '0';
	  name[2] = regno % 10 + '0';
	  namelen = 3;
	}
    }

  name[namelen++] = '\0';
  return namelen;
}
