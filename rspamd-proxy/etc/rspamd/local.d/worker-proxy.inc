# Heinlein Support SLAC 2024 - Workshop - Sichere Mailcluster mit Rspamd und Spamhaus DQS
# https://github.com/HeinleinSupport/rspamd-slac-2024
#

timeout = 15s;
max_retries = 2;
milter = yes; # Enable milter mode

bind_socket = "127.0.0.1:11332";
bind_socket = "[::1]:11332";

upstream "scan" {
  default = yes;
  hosts = "round-robin:10.0.3.146:11333:1,10.0.3.147:11333:1";
  key = "j4zcyxp84q47n8quhnmshbgaa5esjqu451hipxam49g6fhm5kpgy"; # Public key for encryption, generated by rspamadm keypair (optional) 
  compression = yes; # Use zstd compression (optional)
}

# set predefined local worker to explicit non default
upstream local {
    default = false;
}



