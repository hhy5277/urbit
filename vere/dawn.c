/* vere/dawn.c
**
** ethereum-integrated pre-boot validation
*/
#include <unistd.h>
#include <curl/curl.h>
#include <uv.h>
#include "all.h"
#include "vere/vere.h"

/* _dawn_curl_alloc(): allocate a response buffer for curl
*/
static size_t
_dawn_curl_alloc(void* dat_v, size_t uni_t, size_t mem_t, uv_buf_t* buf_u)
{
  size_t siz_t = uni_t * mem_t;
  buf_u->base = c3_realloc(buf_u->base, 1 + siz_t + buf_u->len);

  memcpy(buf_u->base + buf_u->len, dat_v, siz_t);
  buf_u->len += siz_t;
  buf_u->base[buf_u->len] = 0;

  return siz_t;
}

/* _dawn_post_json(): POST JSON to url_c
*/
static uv_buf_t
_dawn_post_json(c3_c* url_c, uv_buf_t lod_u)
{
  CURL *curl;
  CURLcode result;
  long cod_l;
  struct curl_slist* hed_u = 0;

  uv_buf_t buf_u = uv_buf_init(c3_malloc(1), 0);

  if ( !(curl = curl_easy_init()) ) {
    fprintf(stderr, "failed to initialize libcurl\n");
    u3_lo_bail();
  }

  hed_u = curl_slist_append(hed_u, "Accept: application/json");
  hed_u = curl_slist_append(hed_u, "Content-Type: application/json");
  hed_u = curl_slist_append(hed_u, "charsets: utf-8");

  // XX require TLS, pin default cert?

  curl_easy_setopt(curl, CURLOPT_URL, url_c);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _dawn_curl_alloc);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&buf_u);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hed_u);

  // note: must be terminated!
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, lod_u.base);

  result = curl_easy_perform(curl);
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &cod_l);

  // XX retry?
  if ( CURLE_OK != result ) {
    fprintf(stderr, "failed to fetch %s: %s\n",
                    url_c, curl_easy_strerror(result));
    u3_lo_bail();
  }
  if ( 300 <= cod_l ) {
    fprintf(stderr, "error fetching %s: HTTP %ld\n", url_c, cod_l);
    u3_lo_bail();
  }

  curl_easy_cleanup(curl);
  curl_slist_free_all(hed_u);

  return buf_u;
}

/* _dawn_oct_to_buf(): +octs to uv_buf_t
*/
static uv_buf_t
_dawn_oct_to_buf(u3_noun oct)
{
  if ( c3n == u3a_is_cat(u3h(oct)) ) {
    u3_lo_bail();
  }

  c3_w len_w  = u3h(oct);
  c3_y* buf_y = c3_malloc(1 + len_w);
  buf_y[len_w] = 0;

  u3r_bytes(0, len_w, buf_y, u3t(oct));

  u3z(oct);
  return uv_buf_init((void*)buf_y, len_w);
}

/* _dawn_buf_to_oct(): uv_buf_t to +octs
*/
static u3_noun
_dawn_buf_to_oct(uv_buf_t buf_u)
{
  u3_noun len = u3i_words(1, (c3_w*)&buf_u.len);

  if ( c3n == u3a_is_cat(len) ) {
    u3_lo_bail();
  }

  return u3nc(len, u3i_bytes(buf_u.len, (const c3_y*)buf_u.base));
}

/* _dawn_eth_rpc(): ethereum JSON RPC with request/response as +octs
*/
static u3_noun
_dawn_eth_rpc(c3_c* url_c, u3_noun oct)
{
  return _dawn_buf_to_oct(_dawn_post_json(url_c, _dawn_oct_to_buf(oct)));
}

/* _dawn_fail(): pre-boot validation failed
*/
static void
_dawn_fail(u3_noun who, u3_noun rac, u3_noun sas)
{
  u3_noun how = u3dc("scot", 'p', u3k(who));
  c3_c* how_c = u3r_string(u3k(how));

  c3_c* rac_c;

  switch (rac) {
    default: c3_assert(0);
    case c3__czar: {
      rac_c = "galaxy";
      break;
    }
    case c3__king: {
      rac_c = "star";
      break;
    }
    case c3__duke: {
      rac_c = "planet";
      break;
    }
    case c3__earl: {
      rac_c = "moon";
      break;
    }
    case c3__pawn: {
      rac_c = "comet";
      break;
    }
  }

  fprintf(stderr, "dawn: invalid keys for %s '%s'\r\n", rac_c, how_c);

  // XX deconstruct sas, print helpful error messages
  u3m_p("pre-boot error", u3t(sas));

  u3z(how);
  free(how_c);
  u3_lo_bail();
}

/* _dawn_need_unit(): produce a value or print error and exit
*/
static u3_noun
_dawn_need_unit(u3_noun nit, c3_c* msg_c)
{
  if ( u3_nul == nit ) {
    fprintf(stderr, "%s\r\n", msg_c);
    // bails, won't return
    u3_lo_bail();
    return u3_none;
  }
  else {
    u3_noun pro = u3k(u3t(nit));
    u3z(nit);
    return pro;
  }
}

/* _dawn_purl(): ethereum gateway url as (unit purl)
*/
static u3_noun
_dawn_purl(u3_noun rac)
{
  u3_noun url;

  if ( 0 == u3_Host.ops_u.eth_c ) {
    if ( c3__czar == rac ) {
      fprintf(stderr, "boot: galaxy requires ethereum gateway via -e\r\n");
      u3_lo_bail();
    }

    url = u3_nul;
  }
  else {
    //  XX call de-purl directly
    //
    u3_noun par = u3v_wish("auru:de-purl:html");
    u3_noun lur = u3i_string(u3_Host.ops_u.eth_c);
    u3_noun rul = u3dc("rush", u3k(lur), u3k(par));

    if ( u3_nul == rul ) {
      if ( c3__czar == rac ) {
        fprintf(stderr, "boot: galaxy requires ethereum gateway via -e\r\n");
        u3_lo_bail();
      }

      url = u3_nul;
    }
    else {
      //  XX revise for de-purl
      //  auru:de-purl:html parses to (pair user purl)
      //  we need (unit purl)
      //
      url = u3nc(u3_nul, u3k(u3t(u3t(rul))));
    }

    u3z(par); u3z(lur); u3z(rul);
  }

  return url;
}

/* _dawn_turf(): override contract domains with -H
*/
static u3_noun
_dawn_turf(c3_c* dns_c)
{
  u3_noun tuf;

  u3_noun par = u3v_wish("thos:de-purl:html");
  u3_noun dns = u3i_string(dns_c);
  u3_noun rul = u3dc("rush", u3k(dns), u3k(par));

  if ( (u3_nul == rul) || (c3n == u3h(u3t(rul))) ) {
    fprintf(stderr, "boot: invalid domain specified with -H %s\r\n", dns_c);
    // bails, won't return
    u3_lo_bail();
    return u3_none;
  }
  else {
    fprintf(stderr, "boot: overriding network domains with %s\r\n", dns_c);
    u3_noun dom = u3t(u3t(rul));
    tuf = u3nc(u3k(dom), u3_nul);
  }

  u3z(par); u3z(dns); u3z(rul);

  return tuf;
}

/* u3_dawn_vent(): validatated boot event
*/
u3_noun
u3_dawn_vent(u3_noun seed)
{
  u3_noun url, bok, pon, zar, tuf, sap;

  u3_noun ship = u3h(seed);
  u3_noun rank = u3do("clan:title", u3k(ship));

  //  load snapshot if exists
  //
  if ( 0 != u3_Host.ops_u.ets_c ) {
    fprintf(stderr, "boot: loading ethereum snapshot\r\n");
    u3_noun raw_snap = u3ke_cue(u3m_file(u3_Host.ops_u.ets_c));
    sap = u3nc(u3_nul, raw_snap);
  }
  else {
    sap = u3_nul;
  }

  url = _dawn_purl(rank);

  //  XX require https?
  //
  c3_c* url_c = ( 0 != u3_Host.ops_u.eth_c ) ?
    u3_Host.ops_u.eth_c :
    "https://ropsten.infura.io/v3/196a7f37c7d54211b4a07904ec73ad87";

  //  pin block number
  //
  if ( c3y == u3_Host.ops_u.etn ) {
    fprintf(stderr, "boot: extracting block from snapshot\r\n");

    bok = _dawn_need_unit(u3do("bloq:snap:dawn", u3k(u3t(sap))),
                          "boot: failed to extract "
                          "block from snapshot");
  }
  else {
    fprintf(stderr, "boot: retrieving latest block\r\n");

    u3_noun oct = u3v_wish("bloq:give:dawn");
    u3_noun kob = _dawn_eth_rpc(url_c, u3k(oct));

    bok = _dawn_need_unit(u3do("bloq:take:dawn", u3k(kob)),
                          "boot: block retrieval failed");
    u3z(oct); u3z(kob);
  }

  {
    //  +hull:constitution:ethe: on-chain state
    //
    u3_noun hul;

    if ( c3y == u3_Host.ops_u.etn ) {
      fprintf(stderr, "boot: extracting public keys from snapshot\r\n");

      hul = _dawn_need_unit(u3dc("hull:snap:dawn", u3k(ship), u3k(u3t(sap))),
                            "boot: failed to extract "
                            "public keys from snapshot");
    }
    else if ( c3__pawn == rank ) {
      //  irrelevant, just bunt +hull
      //
      hul = u3v_wish("*hull:constitution:ethe");
    }
    else {
      u3_noun who;

      if ( c3__earl == rank ) {
        who = u3do("^sein:title", u3k(ship));

        {
          u3_noun seg = u3dc("scot", 'p', u3k(who));
          c3_c* seg_c = u3r_string(seg);

          fprintf(stderr, "boot: retrieving %s's public keys (for %s)\r\n",
                                              seg_c, u3_Host.ops_u.who_c);
          free(seg_c);
          u3z(seg);
        }
      }
      else {
        who = u3k(ship);
        fprintf(stderr, "boot: retrieving %s's public keys\r\n",
                                           u3_Host.ops_u.who_c);
      }

      {
        u3_noun oct = u3dc("hull:give:dawn", u3k(bok), u3k(who));
        u3_noun luh = _dawn_eth_rpc(url_c, u3k(oct));

        hul = _dawn_need_unit(u3dc("hull:take:dawn", u3k(ship), u3k(luh)),
                              "boot: failed to retrieve public keys");
        u3z(oct); u3z(luh);
      }

      u3z(who);
    }

    //  +live:dawn: network state
    //  XX actually make request
    //
    u3_noun liv = u3_nul;
    // u3_noun liv = _dawn_get_json(parent, /some/url)

    fprintf(stderr, "boot: verifying keys\r\n");

    //  (each sponsor=(unit ship) error=@tas)
    //
    u3_noun sas = u3dt("veri:dawn", u3k(seed), u3k(hul), u3k(liv));

    if ( c3n == u3h(sas) ) {
      // bails, won't return
      _dawn_fail(ship, rank, sas);
      return u3_none;
    }

    //  (unit ship): sponsor
    //  produced by +veri:dawn to avoid coupling to +hull structure
    //  XX reconsider
    //
    pon = u3k(u3t(sas));

    u3z(hul); u3z(liv); u3z(sas);
  }

  //  (map ship [=life =pass]): galaxy table
  //
  if ( c3y == u3_Host.ops_u.etn ) {
    fprintf(stderr, "boot: extracting galaxy table from snapshot\r\n");

    zar = _dawn_need_unit(u3do("czar:snap:dawn", u3k(u3t(sap))),
                          "boot: failed to extract "
                          "galaxy table from snapshot");
  }
  else {
    fprintf(stderr, "boot: retrieving galaxy table\r\n");

    u3_noun oct = u3do("czar:give:dawn", u3k(bok));
    u3_noun raz = _dawn_eth_rpc(url_c, u3k(oct));

    zar = _dawn_need_unit(u3do("czar:take:dawn", u3k(raz)),
                          "boot: failed to retrieve galaxy table");
    u3z(oct); u3z(raz);
  }

  //  (list turf): ames domains
  //
  if ( 0 != u3_Host.ops_u.dns_c ) {
    tuf = _dawn_turf(u3_Host.ops_u.dns_c);
  }
  else if ( c3y == u3_Host.ops_u.etn ) {
    fprintf(stderr, "boot: extracting network domains from snapshot\r\n");

    tuf = _dawn_need_unit(u3do("turf:snap:dawn", u3k(u3t(sap))),
                          "boot: failed to extract "
                          "network domains from snapshot");
  }
  else {
    fprintf(stderr, "boot: retrieving network domains\r\n");

    u3_noun oct = u3do("turf:give:dawn", u3k(bok));
    u3_noun fut = _dawn_eth_rpc(url_c, u3k(oct));

    tuf = _dawn_need_unit(u3do("turf:take:dawn", u3k(fut)),
                          "boot: failed to retrieve network domains");
    u3z(oct); u3z(fut);
  }

  u3z(rank);

  //  [%dawn seed sponsor galaxies domains block eth-url snap]
  //
  return u3nc(c3__dawn, u3nq(seed, pon, zar, u3nq(tuf, bok, url, sap)));
}

/* u3_dawn_come(): mine a comet under star (unit)
*/
u3_noun
u3_dawn_come(u3_noun star)
{
  u3_noun seed;

  if ( u3_nul == star ) {
    //  XX ~marzod hardcoded
    //  choose from list, at random, &c
    //
    star = 256;
  }
  else {
    //  XX parse and validate
    //
    u3_noun tar = u3k(u3t(star));
    u3z(star);
    star = tar;
  }

  {
    u3_noun sar = u3dc("scot", 'p', u3k(star));
    c3_c* tar_c = u3r_string(sar);

    fprintf(stderr, "boot: mining a comet under %s\r\n", tar_c);
    free(tar_c);
    u3z(sar);
  }

  {
    c3_w    eny_w[16];
    u3_noun eny;

    c3_rand(eny_w);
    eny = u3i_words(16, eny_w);

    seed = u3dc("come:dawn", u3k(star), u3k(eny));
    u3z(eny);
  }

  {
    u3_noun who = u3dc("scot", 'p', u3k(u3h(seed)));
    c3_c* who_c = u3r_string(who);

    fprintf(stderr, "boot: found comet %s\r\n", who_c);
    free(who_c);
    u3z(who);
  }

  u3z(star);

  return seed;
}