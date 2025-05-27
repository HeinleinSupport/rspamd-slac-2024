> [!IMPORTANT]  
> You can find the follow-up workshop from 2025 here: [https://github.com/HeinleinSupport/rspamd-slac-2025/blob/main/Readme_eng.md](https://github.com/HeinleinSupport/rspamd-slac-2025/blob/main/Readme_eng.md)


> [!TIP]
> Unser Workshop auf deutsch: [https://github.com/HeinleinSupport/rspamd-slac-2024/blob/main/Readme.md](https://github.com/HeinleinSupport/rspamd-slac-2024/blob/main/Readme.md)

<img src="./images/slac2024.png" alt="SLAC 2024 06.-08.05.2024 Berlin"/>


*****

<img src="./images/Heinlein_Logo.webp" alt="Heinlein Support GmbH" width="60%"/>

# Secure mail clusters with Rspamd and Spamhaus DQS

__Workshop at the SLAC 2024__

- Carsten Rosenberg <c.rosenberg@heinlein-support.de>
- Manu Zurmühl <m.zurmuehl@heinlein-support.de>

<br><br>
_Enterprise grade mail-cluster with open-source? YES ;)_

[https://www.heinlein-support.de/blog/enterprise-mail-security-open-source](https://www.heinlein-support.de/blog/enterprise-mail-security-open-source)

*****

- [Secure mail clusters with Rspamd and Spamhaus DQS](#secure-mail-clusters-with-rspamd-and-spamhaus-dqs)
  - [Links to the configs](#links-to-the-configs)
  - [Redundant Postfix cluster](#redundant-postfix-cluster)
  - [Postfix Cluster #1](#postfix-cluster-1)
  - [Postfix Cluster #2](#postfix-cluster-2)
  - [Postfix Cluster #3](#postfix-cluster-3)
  - [Postfix Cluster #4](#postfix-cluster-4)
  - [Encapsulation of individual services in their own systems or containers/CGroups](#encapsulation-of-individual-services-in-their-own-systems-or-containerscgroups)
  - [Realworld sizing](#realworld-sizing)
  - [Rspamd connection](#rspamd-connection)
  - [Quick Walkthrough Postfix and Rspamd Proxy Config](#quick-walkthrough-postfix-and-rspamd-proxy-config)
  - [MX Config](#mx-config)
  - [Hub Config](#hub-config)
  - [Mailout Config](#mailout-config)
  - [Quick Walkthrough Rspamd Proxy Config](#quick-walkthrough-rspamd-proxy-config)
  - [Rspamd - Symbols, modules, plugins, functions](#rspamd---symbols-modules-plugins-functions)
  - [Rspamd - Composites](#rspamd---composites)
  - [Rspamd - Actions](#rspamd---actions)
  - [Rspamd - Force Actions](#rspamd---force-actions)
  - [Why actions directly in the plugins are (often) not useful](#why-actions-directly-in-the-plugins-are-often-not-useful)
  - [Collect and evaluate indicators](#collect-and-evaluate-indicators)
  - [Scoring and policies for spamd](#scoring-and-policies-for-spamd)
  - [Rspamd Settings](#rspamd-settings)
  - [Rspamd - Composites + Force\_Actions + Groups + Settings](#rspamd---composites--force_actions--groups--settings)
    - [Example Antivirus](#example-antivirus)
    - [Example multimap + settings](#example-multimap--settings)
  - [Rspamd Selectors](#rspamd-selectors)
  - [Rspamd Selectors - own selectors in Lua](#rspamd-selectors---own-selectors-in-lua)
  - [Multimap](#multimap)
  - [Ratelimit](#ratelimit)
  - [Reputation](#reputation)
  - [Spamhaus DQS](#spamhaus-dqs)
  - [DKIM](#dkim)
  - [ARC - Authenticated Received Chain](#arc---authenticated-received-chain)
  - [Bonus: Ratelimit diagrams](#bonus-ratelimit-diagrams)
    - [Ratelimit with empty reservoir (burst)](#ratelimit-with-empty-reservoir-burst)
    - [Ratelimit with full reservoir (burst)](#ratelimit-with-full-reservoir-burst)
    - [Ratelimit with several rules](#ratelimit-with-several-rules)


*****

## Links to the configs

- Postfix MX: [https://github.com/HeinleinSupport/rspamd-slac-2024/tree/main/postfix-mx/etc/postfix](https://github.com/HeinleinSupport/rspamd-slac-2024/tree/main/postfix-mx/etc/postfix)
- Postfix Hub: [https://github.com/HeinleinSupport/rspamd-slac-2024/tree/main/postfix-hub/etc/postfix](https://github.com/HeinleinSupport/rspamd-slac-2024/tree/main/postfix-hub/etc/postfix)
- Postfix Mailout: [https://github.com/HeinleinSupport/rspamd-slac-2024/tree/main/postfix-mailout/etc/postfix](https://github.com/HeinleinSupport/rspamd-slac-2024/tree/main/postfix-mailout/etc/postfix)
- Rspamd Proxy: [https://github.com/HeinleinSupport/rspamd-slac-2024/tree/main/rspamd-proxy/etc/rspamd](https://github.com/HeinleinSupport/rspamd-slac-2024/tree/main/rspamd-proxy/etc/rspamd)
- Rspamd Worker: [https://github.com/HeinleinSupport/rspamd-slac-2024/tree/main/rspamd-worker/etc/rspamd/local.d](https://github.com/HeinleinSupport/rspamd-slac-2024/tree/main/rspamd-worker/etc/rspamd/local.d)

*****

## Redundant Postfix cluster

- In a Postfix infrastructure, we like to separate the systems with Internet communication and internal connections
- Ideally, external systems should not have access to internal systems (or only to the internal Postfix)
- internal mails should not pass via systems connected to the Internet
- MX server (incoming) and mailout (outgoing) are therefore in a DMZ
- HUB (internal mail distributor) is located in the internal network and communicates with MX and Mailout

*****

## Postfix Cluster #1

<img src="./images/Rspamd_Cluster.png" alt="drawing" width="100%"/>

*****

## Postfix Cluster #2

<img src="./images/Rspamd_Cluster_2.png" alt="drawing" width="100%"/>

*****

## Postfix Cluster #3

<img src="./images/Rspamd_Cluster_3.png" alt="drawing" width="100%"/>

*****

## Postfix Cluster #4

<img src="./images/Rspamd_Cluster_4.png" alt="drawing" width="100%"/>

*****

## Encapsulation of individual services in their own systems or containers/CGroups

- Redis is not happy if the working memory in the system runs out
- File analysis tools such as anti-virus systems or VBA/PDF analyses can crash during analysis
- That's why we like to lock these tools in extra systems, containers or with other mechanisms

*****

## Realworld sizing

- We do not necessarily need __xx__ VMs for an effective cluster
- the ideas behind segmentation/splitting can also be implemented with 2 systems with high availability
- it is mainly about minimizing possible security problems and building an effective redundant cluster

*****

## Rspamd connection

- To avoid single points of failure, we install the Rspamd proxy directly on the Postfix system
- The proxy receives a list of Rspamd backends that are addressed in a load-balanced manner
- Advantage: Redundancy and reliability
- Each Postfix passes a keyword for its task in the cluster via `milter_macro_daemon_name`
  - e.g. _incoming_

*****

## Quick Walkthrough Postfix and Rspamd Proxy Config

- Postfix only takes care of routing, static address rewriting and some RFC checks
- TLS should be enforced - yes, even if it is against the RFCs ;)
- Access maps are defined but empty and only intended for emergencies
- Only small differences for MX, hub (internal router), mailout server
- Database connection best only on the internal hubs
- Use of ASCII lists (texthash) as lookup tables
  - nowadays too little data for an indexed database
  - postmap cannot be forgotten ;)

*****

## MX Config

- Configs: relay_domains, verify, TLS exceptions if necessary

```conf
smtpd_tls_security_level = may
smtp_tls_security_level = encrypt

# Postfix Lookup Tables / Maps / Databases
relay_domains = texthash:/etc/postfix/maps.d/relay_domains.list
transport_maps = texthash:/etc/postfix/maps.d/transport.list, $relay_domains
smtp_tls_policy_maps = texthash:/etc/postfix/maps.d/tls_policy.list

# Postfix Restrictions
smtpd_recipient_restrictions = 
# Whitelist postmaster!
  check_recipient_access inline:{
    {postmaster@=permit_mynetworks,permit_sasl_authenticated,reject_unauth_destination,permit}
    {abuse@=permit_mynetworks,permit_sasl_authenticated,reject_unauth_destination,permit}
    },
# whitelist recipients?
  check_recipient_access texthash:/etc/postfix/maps.d/access_recipient.list,
# blacklist hosts and senders?
  check_client_access cidr:/etc/postfix/maps.d/access_client.cidr,
  check_sender_access texthash:/etc/postfix/maps.d/access_sender.list,


# Force TLS - but allow exceptions
  check_sender_access pcre:/etc/postfix/maps.d/access_sender_tls_exclude.pcre,


# Do not accept unclean mails!
  reject_non_fqdn_sender,
  reject_non_fqdn_recipient,
  reject_unknown_sender_domain,
  reject_unknown_recipient_domain,
# Allow our children!
  permit_mynetworks,
# Prohibit all other relaying!
  reject_unauth_destination,
# Dynamic recipient validation
  reject_unverified_recipient,
# What is still allowed through!
  permit
```

- /etc/postfix/maps.d/access_sender_tls_exclude.pcre

```conf
# exclude @mailexample.de from the TLS enforcement
/.*@mailexample.de$/i DUNNO

/.*/ reject_plaintext_session
```

*****

## Hub Config

- Configs: relay_domains, transport, mynetworks, virtual, verify, possibly TLS exceptions

```conf
smtpd_tls_security_level = encrypt
smtp_tls_security_level = encrypt

# Postfix Lookup Tables / Maps / Databases
relay_domains = texthash:/etc/postfix/maps.d/relay_domains.list
transport_maps = texthash:/etc/postfix/maps.d/transport.list, $relay_domains
smtp_tls_policy_maps = texthash:/etc/postfix/maps.d/tls_policy.list
lmtp_tls_policy_maps = texthash:/etc/postfix/maps.d/tls_policy.list
virtual_alias_maps = texthash:/etc/postfix/maps.d/virtual_alias.list

# Postfix Restrictions
smtpd_recipient_restrictions = 
# Whitelist postmaster!
  check_recipient_access inline:{
    {postmaster@=permit_mynetworks,permit_sasl_authenticated,reject_unauth_destination,permit}
    {abuse@=permit_mynetworks,permit_sasl_authenticated,reject_unauth_destination,permit}
    },
# whitelist recipients?
  check_recipient_access texthash:/etc/postfix/maps.d/access_recipient.list,
# blacklist hosts and senders?
  check_client_access cidr:/etc/postfix/maps.d/access_client.cidr,
  check_sender_access texthash:/etc/postfix/maps.d/access_sender.list,
# Do not accept unclean mails!
  reject_non_fqdn_sender,
  reject_non_fqdn_recipient,
  reject_unknown_sender_domain,
  reject_unknown_recipient_domain,
# Allow our children!
  permit_sasl_authenticated,
  permit_mynetworks,
# Prohibit all other relaying!
  reject_unauth_destination,
# Dynamic recipient validation
  reject_unverified_recipient,
# What is still allowed through!
  permit
```

*****

## Mailout Config

- Configs: transport, mynetworks, possibly TLS exceptions and tightening (Dane)

```conf
smtpd_tls_security_level = encrypt
smtp_tls_security_level = encrypt

# Postfix Lookup Tables / Maps / Databases
transport_maps = texthash:/etc/postfix/maps.d/transport.list
smtp_tls_policy_maps = texthash:/etc/postfix/maps.d/tls_policy.list

# Postfix Restrictions
smtpd_recipient_restrictions = 
# Whitelist postmaster!
  check_recipient_access inline:{
    {postmaster@=permit_mynetworks,permit_sasl_authenticated,reject_unauth_destination,permit}
    {abuse@=permit_mynetworks,permit_sasl_authenticated,reject_unauth_destination,permit}
    },
# whitelist recipients?
  check_recipient_access texthash:/etc/postfix/maps.d/access_recipient.list,
# blacklist hosts and senders?
  check_client_access cidr:/etc/postfix/maps.d/access_client.cidr,
  check_sender_access texthash:/etc/postfix/maps.d/access_sender.list,
# Allow our children!
  permit_mynetworks,
# What is still allowed through!
  permit

```

*****

## Quick Walkthrough Rspamd Proxy Config

- Rspamd Proxy is not a standalone package
- the complete Rspamd will be installed
- but we only need a few customizations for actions, logging, timeouts and the Rspamd backend config
- everything else is switched off in the `rspamd.conf.local.override

/etc/rspamd/rspamd.conf.local.override:

```conf
modules {
  path = "/var/lib/rspamd/modules";
  fallback_path = "/var/lib/rspamd/modules"; # Legacy path
}
lua = "/var/lib/rspamd/modules/nil.lua";

antivirus { enabled=false; }
arc { enabled=false; }
asn { enabled=false; }
aws_s3 { enabled=false; }

...

```

/etc/rspamd/local.d/worker-proxy.inc:

```conf
bind_socket = "127.0.0.1:11332";
bind_socket = "[::1]:11332";

upstream "scan" {
  default = yes;
  hosts = "round-robin:10.0.3.146:11333:1,10.0.3.147:11333:1";
  key = "j4zcyxp84q47n8quhnmshbgaa5esjqu451hipxam49g6fhm5kpgy"; 
  compression = yes;
}

```

*****

## Rspamd - Symbols, modules, plugins, functions

- Rspamd thinks in symbols
- Symbols in Rspamd are like objects that can have functions to be executed, configurations, descriptions, points
- Symbols can be switched on and off at runtime, logically linked or used for forced actions
- Symbols can have dependencies on other symbols in the main phase (not pre-filter/post-filter)
  - e.g. DMARC is only executed once SPF and DKIM have been checked
- Symbols can be assigned to one or more groups

<img src="./images/rspamd-symbol.svg" alt="drawing" width="100%"/>

*****

## Rspamd - Composites

- work like the Meta Rules in Spamassassin and can do a lot more
- Matching takes place as a logical expression to activate symbols or groups
  - `expression = "INCOMING & BAD_SUBJECT";`
- If a logical expression is `true`
  - new symbol and points can be added
  - matched symbols or their score can be removed

*****

## Rspamd - Actions

- Rspamd knows various actions that are triggered when the sum of all symbols exceeds a threshold
  - no action
  - greylist (soft reject)
  - add header (this is not directly about headers)
  - rewrite subject
  - reject
- depending on the MTA, you can also define your own actions (Postfix/Milter e.g. discard, quarantine)

[https://rspamd.com/doc/configuration/metrics.html](https://rspamd.com/doc/configuration/metrics.html)

*****

## Rspamd - Force Actions

- in the force actions, logical expressions of symbols can also be used to trigger actions independent of threshold values
  - `expression = "CLAMAV_VIRUS & !WHITELIST_ANTIVIRUS";`
- all actions defined in actions can be used
  - Attention: a `reject = null;` switches off the reject action completely
- Direct actions can also be triggered directly in various plugins
  - Antivirus
  - Multimap
  - DMARC
  - rate limit
  - Spamtrap

[https://rspamd.com/doc/modules/force_actions.html](https://rspamd.com/doc/modules/force_actions.html)

*****

## Why actions directly in the plugins are (often) not useful

- Exceptions to the rejects (whitelisting) are only possible with predefined exceptions in the plugin or cannot be implemented at all
- Early rejects trigger a passthrough so that other checks are aborted
  - Unique spam mails may not be learned
- The rejects in the plugins are often executed in a rather generalized manner
  - Antivirus e.g. encrypted mails or error codes
  - Rate limit all limits lead to a soft reject

*****

## Collect and evaluate indicators

- We recommend letting Rspamd collect many indicators and evaluating them at the end of the scan
  - A virus mail may also be recognized as spam and then also learned
- Composites and force actions are used intensively for this purpose
- Exceptions can be implemented easily
- Incoming, outgoing and internal traffic can be differentiated easily
- Viruses can be categorized e.g. as spam (for learning) and malware (simply reject)
- Rate limit rules can be evaluated individually
  - Rejection for certain rules
  - Info to the admin for other limits
- we use the _Conditional Maps_ or _Combined maps_ options only, if the multimaps are very complex 

*****

## Scoring and policies for spamd

- we often see that unwanted attachments or senders are given a high score in order to be rejected
- However, high scores always trigger the learning mechanisms of Rspamd
  - Bayes, Fuzzy, Reputation, Neural Network, (rate limit)
- You certainly don't want to learn the mail with the `.exe` from your colleagues.
- this would backfire once your signature is recognized as spam ;)
- Rejection for policy reasons: Force actions and at most a low score
- Rejection as SPAM: high score (but please with many indicators)
- Groups can be limited in their maximum score as a safeguard
  - e.g. IP is listed in almost every RBL
- In the case of policy and spam, the force action also works, but the mail is still learned

Example multimaps:

- `SENDER_DOMAIN_BLOCKLIST` -> reject via force_actions
- `SENDER_DOMAIN_SPAM` -> 8.0 points

*****

## Rspamd Settings

- With the settings plugin in Rspamd you can create a scan profile that has different threshold values, deactivates or explicitly activates certain functions
- Or it can simply add another symbol as an indicator for composites or force actions
- the settings profiles can be stored statically in a file, retrieved from a web server, stored in Redis or retrieved from an HTTP API
- Settings profiles have a matching functionality and a priority as well as a section for customization and additional symbols
- We define default symbols according to their location in the infrastructure
  - _incoming_
  - _outgoing_
  - _internal_
- In addition, there are often extra profiles for certain systems or to enable additional exceptions
  - e.g. on the hub - if the mail comes from the MX

```conf
# Default IN - no DKIM sign
INCOMING_DEFAULT {
  id = "INCOMING_DEFAULT";
  priority = low;
 
  request_header = {
    # milter_macro_daemon_name in postfix
    "MTA-Name" = "^incoming_default$";
  }
  apply {
    actions {
      # just an example
      # "rewrite subject" = 13; # Please note the space, NOT an underscore
    }
    symbols_disabled = [
      "DKIM_SIGNED",
    ];
  }
  symbols [
      "INCOMING_DEFAULT",
      "INCOMING"
  ]
}


# Default OUT full + dkim sign
OUTGOING_DEFAULT {
  id = "OUTGOING_DEFAULT";
  priority = low;
  request_header = {
    # milter_macro_daemon_name in postfix
    "MTA-Name" = "^outgoing_default$";
  }
  apply {
    groups_disabled = [
      "dmarc",
    ];
  }
  symbols [
      "OUTGOING_DEFAULT",
      "OUTGOING"
  ]
}
```

[https://rspamd.com/doc/configuration/settings.html](https://rspamd.com/doc/configuration/settings.html)

*****

## Rspamd - Composites + Force_Actions + Groups + Settings

- How do we get from individual indicators to the rejection of a mail?

### Example Antivirus

- Indicator: `CLAMAV_U_PORCUPINE_MALWARE(8.00){Porcupine.Malware.58486.UNOFFICIAL;}`
- Instead of the default CLAMAV symbol, we have created our own symbol for this signature using the patterns

/etc/rspamd/local.d/antivirus.conf

```conf
clamav {
  ...
  symbol = "CLAMAV"

  patterns {
    ...
    CLAMAV_U_PORCUPINE_MALWARE = '/^Porcupine\.Malware/i';
  }
}
```

- This new symbol could now be used directly, but this cannot be managed in bulk
- So we use groups

/etc/rspamd/local.d/antivirus_group.conf

```conf
symbols = {
  "CLAMAV_U_PORCUPINE_MALWARE" {
    description = "ClamAV U ^Porcupine.Malware found";
    weight: 8;
    groups: ["clamav_unofficial", "clamav_u_porcupine", "av_virus_reject", "clamav_u_reject"];
    one_shot: true;
  }
}
```

- The group `av_virus_reject` can now be used for the reject
- A score of 8 also shows that we actually only see this signature in spam mails
- Matching the group in composites

/etc/rspamd/local.d/composites.conf

```conf
GROUP_VIRUS_REJECT {
  expression = "g:av_virus_reject";
  score = 0.0;
  policy = "leave";
  description = "Found a VIRUS_REJECT symbol";
}
```

- Rejection of the mail then in the force actions

/etc/rspamd/local.d/force_actions.conf

```conf
rules {
  VIRUS_REJECT {
    action = "reject";
    expression = "GROUP_VIRUS_REJECT";
    message = "REJECT - virus found (support-id: ${queueid}-${uid.substring(1, 6)})";
    require_action = ["no action", "greylist", "reject", "add header", "soft reject", "rewrite subject", "discard", "quarantine"];
  }
}
```

[https://rspamd.com/doc/modules/antivirus.html](https://rspamd.com/doc/modules/antivirus.html)

*****

### Example multimap + settings

- Rejection of certain attachments for incoming mails and if the recipient is not on the exclude list
- Indicator: `BANNED_EXTENSIONS(0.00){exe;}`

/etc/rspamd/local.d/multimap.conf

```conf
BANNED_EXTENSIONS {
  # Map banned_extensions.map Example:
  # exe
  # scr

  type = "filename";
  filter = "extension";
  map = "file://$LOCAL_CONFDIR/local.d/maps.d/banned_extensions.map";
  symbol = "BANNED_EXTENSIONS";
  score = 1.0;
  message = "A restricted file type was found";
  #skip_archives = true;
}
```

- We do not need composites here, as we do not match on groups
- Rejection of the mail in the force actions
- the other two symbols come from the settings (INCOMING) and another multimap (WL_RCPT_BANNED_EXTENSIONS)

```conf
rules {
  BANNED_EXTENSIONS {
    action = "reject";
    expression = "INCOMING & BANNED_EXTENSIONS & !WL_RCPT_BANNED_EXTENSIONS";
    message = "REJECT - policy violation - attachment type is forbidden (support-id: ${queueid}-${uid.substring(1, 6)})";

  }
}
```

[https://rspamd.com/doc/configuration/composites.html](https://rspamd.com/doc/configuration/composites.html)

*****

## Rspamd Selectors

- Selectors are small functions that can be chained like pipes
- This makes it possible to retrieve and even modify almost any value from a mail or the scan data
- Selectors can often be used in the modules alongside statically defined values (IP, from)
- Own selectors can be written in Lua
- Default modules with selector support
  - multimap
  - ratelimit
  - reputation
  - rbl
  - force_actions (reject message)

Examples:

- Lowercased subject as HEX and reduced to the first 16 characters

```lua
header('Subject').lower.digest('hex').substring(1, 16)
```

- A header value linked to the SMTP-From domain
- `id` would replace the header with a string `.id('test')`
- Effective: returns the SMTP-From domain if `X-SG-EID` exists

```lua
header("X-SG-EID").id;from("smtp", "orig"):domain.get_tld'
```

- SHA256 hashes of all attachments

```lua
attachments(hex,sha256)
```

- Matching on the day of the week linked to the Auth-User
- returns '_usernamework_' or nothing at all

```lua
user.lower;time('connect', '!%w').in(1, 2, 3, 4, 5).id('work')
```

- Values from symbols (options) - here BITCOIN_ADDR

```lua
symbol('BITCOIN_ADDR'):options.first
```

[https://rspamd.com/doc/configuration/selectors.html](https://rspamd.com/doc/configuration/selectors.html)
*****

## Rspamd Selectors - own selectors in Lua

- Source IP - but only if it is an IPv4 address
- Would certainly also work with predefined selectors and regexp filter ;)

```lua
lua_selectors.register_extractor(rspamd_config, "ipv4", {
  get_value = function(task, args)
    local ip = task:get_ip()
    if ip and ip:is_valid() and ip:get_version() == 4 then return ip,'userdata' end
    return nil
  end,
  description = 'Get only ipv4 addresses'
})
```

[https://rspamd.com/doc/lua/lua_selectors.html](https://rspamd.com/doc/lua/lua_selectors.html)
*****

## Multimap

- Activation of a symbol when a query value matches against a map
- Maps can always be local files, files on web servers, Redis data or HTTP API
- Predefined query values (IP, From etc) + filters or selectors
  - You can have virtually any value on the mail or scan matched against a list
- We usually use this as an indicator for policies
  - Blocklists
  - Fraud detection of our domains from outside
  - Extensions
- Rarely for matching for SPAM
- We do not use prefilters (+ reject), conditional maps, combined maps

Example SENDER_IP_BLOCKLIST:__

- pure indicator (for the force actions) without score
- Reject for policy reasons

```conf
SENDER_IP_BLOCKLIST {
  # Map sender_ip_blocklist.map Example:
  # 10.0.0.1
  # 10.2.0.0/16

  type = "ip";
  map = "file://$LOCAL_CONFDIR/local.d/maps.d/sender_ip_blocklist.map";
}
```

__Example SENDER_DOMAIN_SPAMLIST:__

- Spam indicator - we want to drive up the score

```conf
SENDER_DOMAIN_SPAMLIST {
  # Map sender_domain_spamlist.map Example:
  # spamdomain.br
  # nextspammer.shop

  type = "from";
  filter = "email:domain";
  map = "file://$LOCAL_CONFDIR/local.d/maps.d/sender_domain_spamlist.map";
  score = 8.0;
}
```

- Matching the mime content type to S/Mime / PGP content with custom selector
- can be used to re-route the mail

```conf
ENCRYPTED_MIME_PART_CT {
  # Map encrypted_mime_part_ct.map Example:
  # /multipart\/signed;.*/i
  # /application\/pkcs7-mime;.*/i
  # /application\/pgp-keys;.*/i

  type = "selector";
    # Attention 'attachments_ct' is a custom selector
  selector = "attachments_ct.uniq";
  map = "file://$LOCAL_CONFDIR/local.d/maps.d/encrypted_mime_part_ct.map";
  symbol = "ENCRYPTED_MIME_PART_CT";
  regexp = true;
}
```

[https://rspamd.com/doc/modules/multimap.html](https://rspamd.com/doc/modules/multimap.html)

*****

## Ratelimit

- Ratelimit works according to the token bucket method
- So it is not counted 1:1 but also works with a storage container (burst)
  - Alternatively, for very precise counting, we have built our own generic module (ratecounting)
- This sometimes makes it more difficult to understand when a rate limit has been reached
- The usual procedure here too:
  - We build arbitrary limits but without action only as an indicator
  - We match this via groups (groups.conf) and force actions
- With selectors you can build limits on EVERYTHING ;)
- We no longer use `ham_factor_rate` and `ham_factor_burst` because of bad experiences

```conf
rates {
  ip = {
    # sender IP address
    selector = 'ip';
    # You can define more than one bucket, however, you need to use array syntax only
      bucket = [
        {
          symbol = RATELIMIT_IP_MINUTE;
          burst = 10;
          rate = "20 / 1min";
        },
        {
          symbol = RATELIMIT_IP_HOUR;
          burst = 100;
          rate = "1000 / 1h";
        }
      ]
  }
}

# rate / burst adjustments based on spam result
# we do not change the rates / burst on ham results anymore
max_rate_mult = 10;
max_bucket_mult = 20;

# HAM / SPAM multiplier for rate
ham_factor_rate = "1.00"
spam_factor_rate = "0.96"
# HAM / SPAM multiplier for burst
ham_factor_burst = "1.00"
spam_factor_burst = "0.92"

```

[https://rspamd.com/doc/modules/ratelimit.html](https://rspamd.com/doc/modules/ratelimit.html)
*****

## Reputation

- Reputation calculates the average points of the past mail scans for a value
- The score for Reputation is then derived from this
- Learns independently and is a very nice automatic additional indicator for smaller scores
  - Scores are assigned in groups.conf
- With selectors you can build reputations on ANYTHING (e.g. X-Mailer)

```conf

rules {

  ip_reputation = {
    selector "ip" {
    }
    backend "redis" {
    }
    symbol = "IP_REPUTATION";
  }

  sender_replyto_reputation = {
    selector "generic" {
      selector = "header('Reply-To')";
    }
    backend "redis" {
    }
    symbol = "SENDER_REPLYTO_REPUTATION";
  }

  bitcoin_reputation = {
    selector "generic" {
      selector = "symbol('BITCOIN_ADDR'):options.first";
    }
    backend "redis" {
    }
    symbol = "BITCOIN_REPUTATION";
  }

}

```

[https://rspamd.com/doc/modules/reputation.html](https://rspamd.com/doc/modules/reputation.html)

*****

## Spamhaus DQS

- Rspamd comes with default configs for Spamhaus ZEN and DBL
- with the (commercial) DQS from Spamhaus these databases are added
  - ZRD - new domains
  - AuthBL - IPs that have been detected with brute force
  - HBL - Hash Blocklist - file, e-mail, crypto wallets, complete URLs
  - Subdomains with DBL
- This must be configured separately in Rspamd
- Spamhaus Configs for this - [https://github.com/spamhaus/rspamd-dqs](https://github.com/spamhaus/rspamd-dqs)
  - We found some room for improvement in the implementation and have rebuilt it a bit ;)
  - Here again: Implementation of the more complex parts as selectors!
  - You can find the required selectors and config at the Rspamd Config

[rbl.conf](https://github.com/HeinleinSupport/rspamd-slac-2024/blob/main/rspamd-worker/etc/rspamd/local.d/rbl.conf)
[rbl_group.conf](https://github.com/HeinleinSupport/rspamd-slac-2024/blob/main/rspamd-worker/etc/rspamd/local.d/rbl_group.conf)
[spamhaus_dqs.lua](https://github.com/HeinleinSupport/rspamd-slac-2024/blob/main/rspamd-worker/etc/rspamd/local.d/lua.d/spamhaus_dqs.lua)

*****

## DKIM

- With DKIM, we very often do not use domain-specific keys at all
- Rspamd knows a fallback method that falls back to a generic key
- Rspamd can also check whether the correct public key for a domain is stored in the DNS before signing
  - Even with dozens of domains, it is possible to control whether or not to sign purely via the DNS
- No real TXT entry needs to be made in the DNS.
- A central entry is set, which is then only "referenced" by all active DKIM domains.
- For security reasons, it is also best to create a backup key and corresponding DNS settings

__Example:__

- Generic DNS entry (no correct DKIM DNS path)

```dns
DKIM001._domainkey IN TXT ( "v=DKIM1; k=ed25519;" 
  "p=jq9RXxO589TEKlnrylc3eFq5x58xBQjRCl/aYdbwQME="
)

```

- CNAME

```dns
DKIM001._domainkey.mailexample.de CNAME DKIM001.dkim.mailexample.de
DKIM002._domainkey.mailexample.de CNAME DKIM002.dkim.mailexample.de

DKIM001._domainkey.example.com CNAME DKIM001.dkim.mailexample.de
DKIM002._domainkey.example.com CNAME DKIM002.dkim.mailexample.de

```

- dkim_signing.conf

```conf
# Default selector to use
selector = "dkim001";

# If true, envelope/header domain mismatch is ignored
allow_hdrfrom_mismatch = true;
allow_hdrfrom_multiple = true;
allow_username_mismatch = true;

sign_authenticated = true;
sign_local = true;
sign_networks = "/etc/rspamd/local.d/maps.d/sign_networks.map"; # or url

use_domain = "header";

# Whether to fallback to global config
try_fallback = true;

# Whether to normalize domains to eSLD
use_esld = false;

# If `true` get pubkey from DNS record and check if it matches private key
check_pubkey = true;

# Set to `false` if you want to skip signing if publick and private keys mismatches
allow_pubkey_mismatch = false;

```

[https://rspamd.com/doc/modules/dkim_signing.html](https://rspamd.com/doc/modules/dkim_signing.html)

*****

## ARC - Authenticated Received Chain

- the idea of repairing or weakening the impact of broken DMARC, DKIM, SPF for forwarding
- and to be able to verify each MTA (hop) on the delivery path by means of a signature
- each MTA refers to the entries of the previous MTAs
- Instance number (i) indicates the order of the ARC headers

- Each MTA attaches its own extra signature with its own key, analogous to DKIM

```email
ARC-Message-Signature: i=1; a=rsa-sha256; c=relaxed/relaxed; d=ncxs.de;
  s=arc; t=1662545894; h=from:from:reply-to:reply-to:subject:subject:date:date:
  message-id:message-id:to:to:cc:mime-version:mime-version:
  content-type:content-type:dkim-signature;
  bh=blG69kNZ8LiIVEQ+94j5HVyLQhIb2l6zTuMg9kGt+S0=;
  b=zfTNg0LOiq6zf+zGRAryC5qd2UGrBMgVDtE5E6NsyYuKkGcflJ+T5nhOYPYcCa4BBY/zB5
  d0napA3sZ4NthPQmfdvERoctIh1GcJyHaVlkShMuD1F8AHyR24d43wvQvxKLqHVfD11jtv
  JLrFDXXbWG21UjQOeHaijzdjG3xjOdVs06tGJhL9pRckLdvpk/SdOS94JoBLNuMouiXXbJ
  mx60jYGNhKSZt0rTcsDzTIhNBTssXZaPzwcjx6X/TN3spYMawx6cE73lY0P/7wMTuUwj16
  4DZnjIMq5CEoerWOnge0e/Hv3Jvgf3p6gCD7Ap8h6yxRwXg49J+Fj/KGAP9KDg==
```

- the current status of SPF, DKIM and DMARC is documented (may have broken in the meantime)

```email
ARC-Authentication-Results: i=1;
  smtpd-in;
  dkim=pass header.d=amazon.de header.s=llktbq2gwxn3x3xrq5ljspgjk2nc5ajv header.b=ggSqgGHR;
  dkim=pass header.d=amazonses.com header.s=ihchhvubuqgjsxyuhssfvqohv7z3u4hn header.b=GsVmLgqr;
  dmarc=pass (policy=quarantine) header.from=amazon.de;
  spf=pass (smtpd-in: domain of 20220907101812f8fb95bae5cc4c4bb24dc960fe80p0eu-C1N7BPP2IWLHN@bounces.amazon.de 
    designates 54.240.1.68 as permitted sender) 
    smtp mailfrom=20220907101812f8fb95bae5cc4c4bb24dc960fe80p0eu-C1N7BPP2IWLHN@bounces.amazon.de
```

- an extra seal only signs the previous ARC headers and their status (valid/invalid)

```email
ARC-Seal: i=1; s=arc; d=ncxs.de; t=1662545894; a=rsa-sha256; cv=none;
  b=VrzjYe+zk8xlADwh1P1qkmRDf+UUBLecv9pAfT79RMPvwm//wcTtqiJYUPz5ObGLtRkFwB
  HWLR1JzSOk2s0mlKX1rsUlO3AdysAWJ5OdEMI4UCCt0E7iDrX+kmzSJB/sR93lMxnnI28C
  5rUNn34vde+S188lMzdfT6Z0m18nMn4piVJWNceo0o+dvOlXkcr5vKarrB4Lrs8u9Hvgd3
  4knVXRxGhgRMYOIOfbWj2ogVok4JaqmeMwy4tPmiLJ+OJ/Z6zTrsieLyVZu9lPxPEiPITf
  +rH2Ttu4G0CX8f5N+c/tCRAlpAt2EdFM60McycbTqx9CL0DztyTIg6TMpjAJ6w==
```

- On the target system, the signature can be used to cryptographically verify each hop
- Even if the DKIM signature is invalid at the end and the SPF is incorrect, it can be ensured that everything was still OK on the 1st external hop
- For instance 5, i.e. the 5th hop, I can cryptographically ensure that DKIM was valid on instance 1, i.e. the 1st external hop
- provided I trust the hop that inserts the ARC signature

<br>

- _We simply use the DKIM keys in ARC as well_
- But we are now signing on behalf of your domain (eSLD)
- Currently a signature on the MX seems to be sufficient

```conf
# Default path to key, can include '$domain' and '$selector' variables
path = "/var/lib/rspamd/dkim/$selector.key";
# Default selector to use
selector = "dkim001";

# If false, inbound messages are not selected for signing
sign_inbound = true;
# If false, messages from local networks are not selected for signing
symbol_sign = "ARC_SIGNED";
# Whether to fallback to global config
try_fallback = true;

# Domain to use for ARC signing: can be "header", "envelope", "recipient"
# or a domain name like "server.example.com"
use_domain = "slac.lxc";

# Symbol to add when message is signed
use_esld = false;
```

[https://rspamd.com/doc/modules/arc.html](https://rspamd.com/doc/modules/arc.html)

*****

## Bonus: Ratelimit diagrams

<img src="images/rspamd_ratelimit_1.png" alt="Ratelimit" />

*****

### Ratelimit with empty reservoir (burst)

<img src="images/rspamd_ratelimit_2.png" alt="Ratelimit" />

*****

### Ratelimit with full reservoir (burst)

<img src="images/rspamd_ratelimit_3.png" alt="Ratelimit" />

*****

### Ratelimit with several rules

<img src="images/rspamd_ratelimit_4.png" alt="Ratelimit" />

*****
