#ifndef L1_SACCH_H
#define L1_SACCH_H
extern void sacch_tx_callback( gas_id_t gas_id );
extern void sacch_rx_callback(gl1_msg_decode_rpt *rpt, gas_id_t gas_id );
extern void sacch26_metrics_callback( gl1_msg_metrics_rpt *rpt[], gas_id_t gas_id );
extern void sacch51_metrics_callback( gl1_msg_metrics_rpt *rpt[], gas_id_t gas_id );

extern void sacch_tx_callback( gas_id_t gas_id );
extern void sacch_rx_callback(gl1_msg_decode_rpt *rpt, gas_id_t gas_id );
extern void sacch51_metrics_callback( gl1_msg_metrics_rpt *rpt[], gas_id_t gas_id );

#endif