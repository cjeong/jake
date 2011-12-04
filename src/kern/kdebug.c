/**
 * @file kdebug.c
 * @desc
 */
#include <stab.h>
#include <string.h>
#include <vmmap.h>
#include <assert.h>
#include <kern/kdebug.h>

extern const struct stab __STAB_BEGIN__[];  /* beginning of stabs table */
extern const struct stab __STAB_END__[];    /* end of stabs table */
extern const char __STABSTR_BEGIN__[];      /* beginning of string table */
extern const char __STABSTR_END__[];        /* end of string table */

/* stab_binsearch(stabs, region_left, region_right, type, addr):
   some stab types are arranged in increasing order by instruction
   address; for example, N_FUN stabs (stab entries with n_type ==
   N_FUN), which mark functions, and N_SO stabs, which mark source files

   given an instruction address, this function finds the single stab
   entry of type 'type' that contains that address

   the search takes place within the range [*region_left, *region_right];
   thus, to search an entire set of N stabs, you might do:

     left = 0;
     right = N - 1;     (* rightmost stab *)
     stab_binsearch(stabs, &left, &right, type, addr);

   the search modifies *region_left and *region_right to bracket the
   'addr'; *region_left points to the matching stab that contains
   'addr', and *region_right points just before the next stab;  If
   *region_left > *region_right, then 'addr' is not contained in any
   matching stab

   for example, given these N_SO stabs:

	   index  type   address
		 0      SO     f0100000
		 13     SO     f0100040
		 117    SO     f0100176
		 118    SO     f0100178
		 555    SO     f0100652
		 556    SO     f0100654
		 657    SO     f0100849

	 this code:

	   left = 0, right = 657;
		 stab_binsearch(stabs, &left, &right, N_SO, 0xf0100184);

	 will exit setting left = 118, right = 554 */
static void stab_binsearch(const struct stab *stabs, int *region_left, 
                           int *region_right, int type, uintptr_t addr)
{
  int l = *region_left, r = *region_right, any_matches = 0;
  
  while (l <= r) {
    int true_m = (l + r) / 2, m = true_m;
    
    // search for earliest stab with right type
    while (m >= l && stabs[m].n_type != type)
      m--;
    if (m < l) {  // no match in [l, m]
      l = true_m + 1;
      continue;
    }

    // actual binary search
    any_matches = 1;
    if (stabs[m].n_value < addr) {
      *region_left = m;
      l = true_m + 1;
    } 
		else if (stabs[m].n_value > addr) {
      *region_right = m - 1;
      r = m - 1;
    } 
		else {
      /* exact match for 'addr', but continue loop to find *region_right */
      *region_left = m;
      l = m;
      addr++;
    }
  }

  if (!any_matches)
    *region_right = *region_left - 1;
  else {
    /* find rightmost region containing 'addr' */
    for (l = *region_right;
         l > *region_left && stabs[l].n_type != type;
         l--)
      /* do nothing */;
    *region_left = l;
  }
}


/* fill in the 'info' structure with information about the specified
  instruction address, 'addr';  returns 0 if information was found, and
  negative if not; but even if it returns negative it has stored some
  information into '*info' */
int debuginfo_eip(uintptr_t addr, struct Eipdebuginfo *info)
{
  const struct stab *stabs, *stab_end;
  const char *stabstr, *stabstr_end;
  int lfile, rfile, lfun, rfun, lline, rline;

  /* initialize *info */
  info->eip_file = "<unknown>";
  info->eip_line = 0;
  info->eip_fn_name = "<unknown>";
  info->eip_fn_namelen = 9;
  info->eip_fn_addr = addr;
  info->eip_fn_narg = 0;

  /* find the relevant set of stabs */
  if (addr >= ULIM) {
    stabs = __STAB_BEGIN__;
    stab_end = __STAB_END__;
    stabstr = __STABSTR_BEGIN__;
    stabstr_end = __STABSTR_END__;
  } 
	else
    /* can't search for user-level addresses yet! */
		panic("User address");

  /* string table validity checks */
  if (stabstr_end <= stabstr || stabstr_end[-1] != 0)
    return -1;

  /* now we find the right stabs that define the function containing
		 'eip'; first, we find the basic source file containing 'eip';
		 then, we look in that source file for the function; then we look
		 for the line number */
  
  /* search the entire set of stabs for the source file (type N_SO) */
  lfile = 0;
  rfile = (stab_end - stabs) - 1;
  stab_binsearch(stabs, &lfile, &rfile, N_SO, addr);
  if (lfile == 0)
    return -1;

  /* search within that file's stabs for the function definition (N_FUN) */
  lfun = lfile;
  rfun = rfile;
  stab_binsearch(stabs, &lfun, &rfun, N_FUN, addr);

  if (lfun <= rfun) {
    /* stabs[lfun] points to the function name in the string table, but 
			 check bounds just in case; */
    if (stabs[lfun].n_strx < stabstr_end - stabstr)
      info->eip_fn_name = stabstr + stabs[lfun].n_strx;
    info->eip_fn_addr = stabs[lfun].n_value;
    addr -= info->eip_fn_addr;
    /* search within the function definition for the line number */
    lline = lfun;
    rline = rfun;
  } 
	else {
    /* couldn't find function stab! maybe we're in an assembly file; 
			 search the whole file for the line number */
    info->eip_fn_addr = addr;
    lline = lfile;
    rline = rfile;
  }

  /* ignore stuff after the colon */
  info->eip_fn_namelen = strfind(info->eip_fn_name, ':') - info->eip_fn_name;

  /* search within [lline, rline] for the line number stab; if found, 
		 set info->eip_line to the right line number; if not found, return -1

		 HINT: there's a particular stabs type used for line numbers; look at 
     the STABS documentation and <inc/stab.h> to find which one */
	
	// Your code here.
  
  /* search backwards from the line number for the relevant filename stab;
		 we can't just use the "lfile" stab because inlined functions can 
		 interpolate code from a different file! such included source files use 
		 the N_SOL stab type */
  while (lline >= lfile && stabs[lline].n_type != N_SOL
         && (stabs[lline].n_type != N_SO || !stabs[lline].n_value))
    lline--;
  if (lline >= lfile && stabs[lline].n_strx < stabstr_end - stabstr)
    info->eip_file = stabstr + stabs[lline].n_strx;


  /* set eip_fn_narg to the number of arguments taken by the function,
		 or 0 if there was no containing function */
  if (lfun < rfun) {
    for (lline = lfun + 1; lline < rfun && stabs[lline].n_type == N_PSYM;
         lline++)
      info->eip_fn_narg++;
	}
  
  return 0;
}
