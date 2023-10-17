/* SPDX-FileCopyrightText: 2023 Greenbone AG
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include <stdlib.h>
#include "program_args_utils.h"

void free_program_args(progr_args_t *pa) {

  if (pa == NULL)
    return;
  
  if (pa->hostname){
    free(pa->hostname);
    pa->hostname = NULL;
  }
  if (pa->query) {
    free(pa->query);
    pa->query = NULL;
  }
  if (pa->delim){
    free(pa->delim);
    pa->delim = NULL;
  }
  if (pa->ns){
    free(pa->ns);
    pa->ns = NULL;
  }
  free(pa);
  pa = NULL;
}

progr_args_t *
init_program_args () {
  progr_args_t *args = calloc(1, sizeof(progr_args_t));
  return args;
}
