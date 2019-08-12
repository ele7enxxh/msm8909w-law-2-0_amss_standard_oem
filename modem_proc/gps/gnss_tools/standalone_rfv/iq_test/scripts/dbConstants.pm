package dbConstants;

use constant DIAG_SUBSYS_CMD              => 75;
use constant DIAG_SUBSYS_DATA             => 42;
use constant DPL_RESET_LOGGING_REQ        => 'Data/DPL Reset Request';
use constant DPL_RESET_LOGGING_RSP        => 'Data/DPL Reset Response';
use constant DPL_GET_PROTOCOL_IDS_REQ     => 'Data/DPL Get Supported Protocol IDs Request';
use constant DPL_GET_PROTOCOL_IDS_RSP     => 'Data/DPL Get Supported Protocol IDs Response';
use constant DPL_GET_IFACE_NAME_IDS_REQ   => 'Data/DPL Get Interface Name IDs Request';
use constant DPL_GET_IFACE_NAME_IDS_RSP   => 'Data/DPL Get Interface Name IDs Response';
use constant DPL_GET_IFACE_NAME_DESC_REQ  => 'Data/DPL Get Interface Name Descriptions Request';
use constant DPL_GET_IFACE_NAME_DESC_RSP  => 'Data/DPL Get Interface Name Descriptions Response';
use constant DPL_SET_IFACE_LOGGING_REQ    => 'Data/DPL Set Interface Logging Request';
use constant DPL_SET_IFACE_LOGGING_RSP    => 'Data/DPL Set Interface Logging Response';
use constant EVENT_BREW_APP_START         => 640;
use constant EVENT_BREW_APP_STOP          => 641;
use constant EVENT_BREW_APP_PAUSE         => 642;
use constant EVENT_BREW_APP_RESUME        => 643;
use constant MSG_LOW                      => 0;
use constant MSG_MEDIUM                   => 1;
use constant MSG_HIGH                     => 2;
use constant MSG_ERROR                    => 3;
use constant MSG_FATAL                    => 4;
use constant MSG_ID_AAGPS                 => 24;
use constant MSG_ID_SEARCHER              => 1007;
use constant MSG_ID_GSM_RR                => 4003;

use constant TIMEOUT                      => 500;
use constant ITEM_TYPE_SUBSYS_RX          => 9;

require Exporter;
@ISA = qw( Exporter );
@EXPORT = qw( 
   DIAG_SUBSYS_CMD
   DIAG_SUBSYS_DATA
   DPL_GET_PROTOCOL_IDS_REQ
   DPL_GET_PROTOCOL_IDS_RSP
   DPL_RESET_LOGGING_REQ
   DPL_RESET_LOGGING_RSP
   DPL_GET_IFACE_NAME_IDS_REQ
   DPL_GET_IFACE_NAME_IDS_RSP
   DPL_GET_IFACE_NAME_DESC_REQ
   DPL_GET_IFACE_NAME_DESC_RSP
   DPL_SET_IFACE_LOGGING_REQ
   DPL_SET_IFACE_LOGGING_RSP
   EVENT_BREW_APP_START
   EVENT_BREW_APP_STOP
   EVENT_BREW_APP_PAUSE
   EVENT_BREW_APP_RESUME
);

1;
