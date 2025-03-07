/*
 ***************************************************************************
 * MediaTek Inc.
 *
 * All rights reserved. source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of MediaTek. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of MediaTek, Inc. is obtained.
 ***************************************************************************
*/

#include "rt_config.h"
#include "chip/mt7626_cr.h"
#include "hdev/hdev.h"

static INT pse_pg_cnt[] = {256, 512, 512, 1024, 768, 1536, 1024, 2048, 1280, 2560, 1536, 3072, 1792, 3584, 0, 0};
static EMPTY_QUEUE_INFO_T Queue_Empty_info[] = {
	{"CPU Q0",  ENUM_UMAC_CPU_PORT_1,     ENUM_UMAC_CTX_Q_0},
	{"CPU Q1",  ENUM_UMAC_CPU_PORT_1,     ENUM_UMAC_CTX_Q_1},
	{"CPU Q2",  ENUM_UMAC_CPU_PORT_1,     ENUM_UMAC_CTX_Q_2},
	{"CPU Q3",  ENUM_UMAC_CPU_PORT_1,     ENUM_UMAC_CTX_Q_3},
	{NULL, 0, 0}, {NULL, 0, 0}, {NULL, 0, 0}, {NULL, 0, 0}, /* 4~7 not defined */
	{"ALTX Q0", ENUM_UMAC_LMAC_PORT_2,    ENUM_UMAC_LMAC_PLE_TX_Q_ALTX_0}, /* Q16 */
	{"BMC Q0",  ENUM_UMAC_LMAC_PORT_2,    ENUM_UMAC_LMAC_PLE_TX_Q_BMC_0},
	{"BCN Q0",  ENUM_UMAC_LMAC_PORT_2,    ENUM_UMAC_LMAC_PLE_TX_Q_BNC_0},
	{"PSMP Q0", ENUM_UMAC_LMAC_PORT_2,    ENUM_UMAC_LMAC_PLE_TX_Q_PSMP_0},
	{"ALTX Q1", ENUM_UMAC_LMAC_PORT_2,    ENUM_UMAC_LMAC_PLE_TX_Q_ALTX_1},
	{"BMC Q1",  ENUM_UMAC_LMAC_PORT_2,    ENUM_UMAC_LMAC_PLE_TX_Q_BMC_1},
	{"BCN Q1",  ENUM_UMAC_LMAC_PORT_2,    ENUM_UMAC_LMAC_PLE_TX_Q_BNC_1},
	{"PSMP Q1", ENUM_UMAC_LMAC_PORT_2,    ENUM_UMAC_LMAC_PLE_TX_Q_PSMP_1},
	{"NAF Q",   ENUM_UMAC_LMAC_PORT_2,    ENUM_UMAC_LMAC_PLE_TX_Q_NAF},
	{"NBCN Q",  ENUM_UMAC_LMAC_PORT_2,    ENUM_UMAC_LMAC_PLE_TX_Q_NBCN},
	{NULL, 0, 0}, {NULL, 0, 0}, {NULL, 0, 0}, {NULL, 0, 0}, {NULL, 0, 0}, {NULL, 0, 0},
	{NULL, 0, 0}, {NULL, 0, 0}, {NULL, 0, 0}, {NULL, 0, 0}, {NULL, 0, 0}, {NULL, 0, 0}, /* 18~29 not defined */
	{"RLS Q",   ENUM_PLE_CTRL_PSE_PORT_3, ENUM_UMAC_PLE_CTRL_P3_Q_0X1E},
	{"RLS2 Q",  ENUM_PLE_CTRL_PSE_PORT_3, ENUM_UMAC_PLE_CTRL_P3_Q_0X1F}
};
static PCHAR sta_ctrl_reg[] = {"ENABLE", "DISABLE", "PAUSE"};

static INT32 chip_dump_ps_table(struct hdev_ctrl *ctrl, UINT32 ent_type, BOOLEAN bReptCli)
{
	RTMP_ADAPTER *pAd = ctrl->priv;
	int i;
	UINT32 ps_stat[4] = {0};
	UINT32 regValue = 0;
	UINT8 bmc_cnt[4] = {0};
	UINT8 ext_bmc_cnt[15] = {0};

	HW_IO_READ32(pAd->hdev_ctrl, PLE_STATION_PAUSE0, &ps_stat[0]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_STATION_PAUSE1, &ps_stat[1]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_STATION_PAUSE2, &ps_stat[2]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_STATION_PAUSE3, &ps_stat[3]);
	HW_IO_READ32(pAd->hdev_ctrl, ARB_BMCCR0, &regValue);
	bmc_cnt[0] = regValue & 0xff;
	bmc_cnt[1] = (regValue & 0xff00) >> 8;
	bmc_cnt[2] = (regValue & 0xff0000) >> 16;
	bmc_cnt[3] = (regValue & 0xff000000) >> 24;
	HW_IO_READ32(pAd->hdev_ctrl, ARB_BMCCR1, &regValue);
	ext_bmc_cnt[0] = regValue & 0xff;
	ext_bmc_cnt[1] = (regValue & 0xff00) >> 8;
	ext_bmc_cnt[2] = (regValue & 0xff0000) >> 16;
	ext_bmc_cnt[3] = (regValue & 0xff000000) >> 24;
	HW_IO_READ32(pAd->hdev_ctrl, ARB_BMCCR2, &regValue);
	ext_bmc_cnt[4] = regValue & 0xff;
	ext_bmc_cnt[5] = (regValue & 0xff00) >> 8;
	ext_bmc_cnt[6] = (regValue & 0xff0000) >> 16;
	ext_bmc_cnt[7] = (regValue & 0xff000000) >> 24;
	HW_IO_READ32(pAd->hdev_ctrl, ARB_BMCCR3, &regValue);
	ext_bmc_cnt[8] = regValue & 0xff;
	ext_bmc_cnt[9] = (regValue & 0xff00) >> 8;
	ext_bmc_cnt[10] = (regValue & 0xff0000) >> 16;
	ext_bmc_cnt[11] = (regValue & 0xff000000) >> 24;
	HW_IO_READ32(pAd->hdev_ctrl, ARB_BMCCR4, &regValue);
	ext_bmc_cnt[12] = regValue & 0xff;
	ext_bmc_cnt[13] = (regValue & 0xff00) >> 8;
	ext_bmc_cnt[14] = (regValue & 0xff0000) >> 16;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("PS_info:\n"));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("%08x %08x %08x %08x\n", ps_stat[0], ps_stat[1], ps_stat[2],
			 ps_stat[3]));

	for (i = 0; i < 4; i++) {
		if (bmc_cnt[i])
			MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("BMC(%x)%d ", i, bmc_cnt[i]));
	}

	for (i = 0; i < 15; i++) {
		if (ext_bmc_cnt[i])
			MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("BMC(%x)%d ", i + 0x11, ext_bmc_cnt[i]));
	}

	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\n\n"));
	/* Dump PS info from FW */
	CmdExtGeneralTestAPPWS(pAd, APPWS_ACTION_DUMP_INFO);
	return TRUE;
}

static INT32 chip_show_tmac_info(struct hdev_ctrl *ctrl, RTMP_STRING *arg)
{
	UINT32 Value;
	RTMP_ADAPTER *pAd = ctrl->priv;

	RTMP_IO_READ32(pAd->hdev_ctrl, TMAC_TCR, &Value);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("TX Stream = %d\n", GET_TMAC_TCR_TX_STREAM_NUM(Value) + 1));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("TX RIFS Enable = %d\n", GET_TX_RIFS_EN(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("RX RIFS Mode = %d\n", GET_RX_RIFS_MODE(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("TXOP TBTT Control = %d\n", GET_TXOP_TBTT_CONTROL(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("TXOP TBTT Stop Control = %d\n", GET_TBTT_TX_STOP_CONTROL(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("TXOP Burst Stop = %d\n", GET_TXOP_BURST_STOP(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("RDG Mode = %d\n", GET_RDG_RA_MODE(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("RDG Responser Enable = %d\n", GET_RDG_RESP_EN(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("Smoothing = %d\n", GET_SMOOTHING(Value)));
	RTMP_IO_READ32(pAd->hdev_ctrl, TMAC_PSCR, &Value);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("AP Power Save RXPE Off Time(unit 2us) = %d\n",
			 GET_APS_OFF_TIME(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("AP Power Save RXPE On Time(unit 2us) = %d\n", APS_ON_TIME(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("AP Power Save Halt Time (unit 32us) = %d\n",
			 GET_APS_HALT_TIME(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("AP Power Enable = %d\n", GET_APS_EN(Value)));
	RTMP_IO_READ32(pAd->hdev_ctrl, TMAC_ACTXOPLR1, &Value);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("AC0 TXOP = 0x%x (unit: 32us)\n", GET_AC0LIMIT(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("AC1 TXOP = 0x%x (unit: 32us)\n", GET_AC1LIMIT(Value)));
	RTMP_IO_READ32(pAd->hdev_ctrl, TMAC_ACTXOPLR0, &Value);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("AC2 TXOP = 0x%x (unit: 32us)\n", GET_AC2LIMIT(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("AC3 TXOP = 0x%x (unit: 32us)\n", GET_AC3LIMIT(Value)));
	RTMP_IO_READ32(pAd->hdev_ctrl, TMAC_ACTXOPLR3, &Value);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("AC10 TXOP = 0x%x (unit: 32us)\n", GET_AC10LIMIT(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("AC11 TXOP = 0x%x (unit: 32us)\n", GET_AC11LIMIT(Value)));
	RTMP_IO_READ32(pAd->hdev_ctrl, TMAC_ACTXOPLR2, &Value);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("AC12 TXOP = 0x%x (unit: 32us)\n", GET_AC12LIMIT(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("AC13 TXOP = 0x%x (unit: 32us)\n", GET_AC13LIMIT(Value)));
	RTMP_IO_READ32(pAd->hdev_ctrl, TMAC_ICR_BAND_0, &Value);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("EIFS Time, Band0 (unit: 1us) = %d\n", GET_ICR_EIFS_TIME(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("RIFS Time, Band0 (unit: 1us) = %d\n", GET_ICR_RIFS_TIME(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("SIFS Time, Band0 (unit: 1us) = %d\n", GET_ICR_SIFS_TIME(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("SLOT Time, Band0 (unit: 1us) = %d\n", GET_ICR_SLOT_TIME(Value)));
	RTMP_IO_READ32(pAd->hdev_ctrl, TMAC_ICR_BAND_1, &Value);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("EIFS Time, Band1 (unit: 1us) = %d\n", GET_ICR_EIFS_TIME(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("RIFS Time, Band1 (unit: 1us) = %d\n", GET_ICR_RIFS_TIME(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("SIFS Time, Band1 (unit: 1us) = %d\n", GET_ICR_SIFS_TIME(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("SLOT Time, Band1 (unit: 1us) = %d\n", GET_ICR_SLOT_TIME(Value)));
	RTMP_IO_READ32(pAd->hdev_ctrl, TMAC_ATCR, &Value);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("Aggregation Timeout (unit: 50ns) = 0x%x\n", GET_AGG_TOUT(Value)));

/* TMAC debug flag dump */
{
	UINT8 i;
	UINT32 flag = 0x00000101; /* init flag */
	UINT32 addr = 0;
	RTMP_IO_WRITE32(pAd->hdev_ctrl,/*0x820F082C*/ (WF_CFG_BASE + 0x82C), 0xF);
	RTMP_IO_WRITE32(pAd->hdev_ctrl,/*0x80025100*/ (TOP_CFG_BASE + 0x5100), 0x1F);
	RTMP_IO_WRITE32(pAd->hdev_ctrl,/*0x80025104*/ (TOP_CFG_BASE + 0x5104), 0x07070707); /* Set to TMAC */

	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("==================== TMAC DBG =================>\n"));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("0x820F082C = 0x%08x\n", 0xF));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("0x80025100 = 0x%08x\n", 0x1F));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("0x80025104 = 0x%08x\n", 0x07070707));
	for (i = 0 ; i < 34; i++) {
		MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("[cnt %d]0x80025108 = 0x%08x\n", i, flag));
		RTMP_IO_WRITE32(pAd->hdev_ctrl,/*0x80025108*/ (TOP_CFG_BASE + 0x5108), flag);
		RTMP_IO_READ32(pAd->hdev_ctrl,/*0x820F0024*/ (WF_CFG_BASE + 0x24), &Value);
		MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("[cnt %d]0x820F0024 = 0x%08x\n", i, Value));
		flag = flag + 0x02020202;
	}

	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("=================== Others TMAC DBG ==================\n"));

	/* Others dump */
	addr =/*0x820F4000*/ 0;
	while (addr <= /*0x820F423C*/ 0x23C) {
		RTMP_IO_READ32(pAd->hdev_ctrl, (WF_TMAC_BASE + addr), &Value);
		MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("0x%x = 0x%08x\n", (WF_TMAC_BASE + addr), Value));
		addr += 4;
	}

	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("=================== Control Signal TMAC DBG ==================\n"));

	addr =/*0x820F4120*/ 0x120;
	while (addr <= /*0x820F412C*/0x12C) {
		RTMP_IO_READ32(pAd->hdev_ctrl, (WF_TMAC_BASE + addr), &Value);
		MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("0x%x = 0x%08x\n", (WF_TMAC_BASE + addr), Value));
		addr += 4;
	}
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("=================== TxV TMAC DBG ==================\n"));

	/* TxV dump */
	addr =/*0x820F4130*/ 0x130;
	while (addr <= /*0x820D4148*/ 0x148) {
		RTMP_IO_READ32(pAd->hdev_ctrl, (WF_TMAC_BASE + addr), &Value);
		MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("0x%x = 0x%08x\n", (WF_TMAC_BASE + addr), Value));
		addr += 4;
	}
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("<=================== TMAC DBG ==================\n"));
}

	return 0;
}

static INT32 chip_show_agg_info(struct hdev_ctrl *ctrl, RTMP_STRING *arg)
{
	UINT32 Value;
	RTMP_ADAPTER *pAd = ctrl->priv;

	RTMP_IO_READ32(pAd->hdev_ctrl, AGG_PCR, &Value);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("MM Protection = %d\n", GET_MM_PROTECTION(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("GF Protection = %d\n", GET_GF_PROTECTION(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("Protection Mode = %d\n", GET_PROTECTION_MODE(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("BW40 Protection = %d\n", GET_BW40_PROTECTION(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("RIFS Protection = %d\n", GET_RIFS_PROTECTION(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("BW80 Protection = %d\n", GET_BW80_PROTECTION(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("BW160 Protection = %d\n", GET_BW160_PROTECTION(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("ERP Protection = 0x%x\n", GET_ERP_PROTECTION(Value)));
	RTMP_IO_READ32(pAd->hdev_ctrl, AGG_PCR1, &Value);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("RTS Threshold(packet length) = 0x%x\n", GET_RTS_THRESHOLD(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("RTS PKT Nums Threshold = %d\n", GET_RTS_PKT_NUM_THRESHOLD(Value)));
	RTMP_IO_READ32(pAd->hdev_ctrl, AGG_MRCR, &Value);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("RTS Retry Count Limit = %d\n", GET_RTS_RTY_CNT_LIMIT(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("BAR Frame Tx Count Limit = %d\n", GET_BAR_TX_CNT_LIMIT(Value)));
	RTMP_IO_READ32(pAd->hdev_ctrl, AGG_ACR0, &Value);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("AMPDU No BA Rule = %d\n", GET_AMPDU_NO_BA_RULE(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("AMPDU No BA AR Rule = %d\n",
			 GET_AGG_ACR_AMPDU_NO_BA_AR_RULE(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("BAR Tx Rate = 0x%x\n", GET_BAR_RATE_TX_RATE(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("BAR Tx Mode = 0x%x\n", GET_BAR_RATE_TX_MODE(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("BAR Nsts = %d\n", GET_BAR_RATE_NSTS(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("BAR STBC = %d\n", GET_BAR_RATE_STBC(Value)));
	RTMP_IO_READ32(pAd->hdev_ctrl, AGG_AALCR0, &Value);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("AC0 Agg limit = %d\n", GET_AC0_AGG_LIMIT(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("AC1 Agg limit = %d\n", GET_AC1_AGG_LIMIT(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("AC2 Agg limit = %d\n", GET_AC2_AGG_LIMIT(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("AC3 Agg limit = %d\n", GET_AC3_AGG_LIMIT(Value)));
	RTMP_IO_READ32(pAd->hdev_ctrl, AGG_AALCR1, &Value);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("AC10 Agg limit = %d\n", GET_AC10_AGG_LIMIT(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("AC11 Agg limit = %d\n", GET_AC11_AGG_LIMIT(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("AC12 Agg limit = %d\n", GET_AC12_AGG_LIMIT(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("AC13 Agg limit = %d\n", GET_AC13_AGG_LIMIT(Value)));
	RTMP_IO_READ32(pAd->hdev_ctrl, AGG_AWSCR0, &Value);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("Winsize0 limit = %d\n", GET_WINSIZE0(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("Winsize1 limit = %d\n", GET_WINSIZE1(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("Winsize2 limit = %d\n", GET_WINSIZE2(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("Winsize3 limit = %d\n", GET_WINSIZE3(Value)));
	RTMP_IO_READ32(pAd->hdev_ctrl, AGG_AWSCR1, &Value);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("Winsize4 limit = %d\n", GET_WINSIZE4(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("Winsize5 limit = %d\n", GET_WINSIZE5(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("Winsize6 limit = %d\n", GET_WINSIZE6(Value)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("Winsize7 limit = %d\n", GET_WINSIZE7(Value)));

/* AGG debug flag dump */
{
	UINT8 i;
	UINT32 flag = 0x00000101; /* init flag */
	UINT32 addr = 0;
	RTMP_IO_WRITE32(pAd->hdev_ctrl,/*0x820F082C*/ (WF_CFG_BASE + 0x82C), 0xF);
	RTMP_IO_WRITE32(pAd->hdev_ctrl,/*0x80025100*/ (TOP_CFG_BASE + 0x5100), 0x1F);
	RTMP_IO_WRITE32(pAd->hdev_ctrl,/*0x80025104*/ (TOP_CFG_BASE + 0x5104), 0x05050505); /* Set to AGG */

	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("==================== AGG DBG =================>\n"));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("0x820F082C = 0x%08x\n", 0xF));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("0x80025100 = 0x%08x\n", 0x1F));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("0x80025104 = 0x%08x\n", 0x05050505));

	for (i = 0 ; i < 128; i++) {
		MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("[cnt %d]0x80025108 = 0x%08x\n", i, flag));
		RTMP_IO_WRITE32(pAd->hdev_ctrl,/*0x80025108*/(TOP_CFG_BASE + 0x5108), flag);
		RTMP_IO_READ32(pAd->hdev_ctrl,/*0x820F0024*/(WF_CFG_BASE + 0x24), &Value);
		MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("[cnt %d]0x820F0024 = 0x%08x\n", i, Value));
		flag = flag + 0x02020202;
	}

	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("=================== Others AGG DBG ==================\n"));

	/* Others dump */
	addr = 0;/*0x820F2000*/
	while (addr <= /*0x820F21FC*/0x1FC) {
		RTMP_IO_READ32(pAd->hdev_ctrl, (WF_AGG_BASE + addr), &Value);
		MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("0x%x = 0x%08x\n", (WF_AGG_BASE + addr), Value));
		addr += 4;
	}

	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("<=================== AGG DBG ==================\n"));

}

	return 0;
}

/* ARB debug flag dump */
static INT32 chip_show_arb_info(struct hdev_ctrl *ctrl, RTMP_STRING *arg)
{
	UINT32 Value;
	RTMP_ADAPTER *pAd = ctrl->priv;
	UINT8 i;
	UINT32 flag = 0x00000101; /* init flag */
	UINT32 addr = 0;
	RTMP_IO_WRITE32(pAd->hdev_ctrl,/*0x820F082C*/ (WF_CFG_BASE + 0x82C), 0xF);
	RTMP_IO_WRITE32(pAd->hdev_ctrl,/*0x80025100*/ (TOP_CFG_BASE + 0x5100), 0x1F);
	RTMP_IO_WRITE32(pAd->hdev_ctrl,/*0x80025104*/ (TOP_CFG_BASE + 0x5104), 0x04040404); /* Set to ARB*/
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("==================== ARB DBG =================>\n"));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("0x820F082C = 0x%08x\n", 0xF));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("0x80025100 = 0x%08x\n", 0x1F));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("0x80025104 = 0x%08x\n", 0x04040404));
	for (i = 0 ; i < 128; i++) {
		MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("[cnt %d]0x80025108 = 0x%08x\n", i, flag));
		RTMP_IO_WRITE32(pAd->hdev_ctrl,/*0x80025108*/ (TOP_CFG_BASE + 0x5108), flag);
		RTMP_IO_READ32(pAd->hdev_ctrl,/*0x820F0024*/ (WF_CFG_BASE + 0x24), &Value);
		MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("[cnt %d]0x820F0024 = 0x%08x\n", i, Value));
		flag = flag + 0x02020202;
	}

	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("=================== Others ARB DBG ==================\n"));

	/* Others dump */
	addr = 0;/*0x820F3000*/
	while (addr <=/*0x820F33FC*/ 0x3FC) {
		RTMP_IO_READ32(pAd->hdev_ctrl, (WF_ARB_BASE + addr), &Value);
		MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("0x%x = 0x%08x\n", (WF_ARB_BASE + addr), Value));
		addr += 4;
	}

	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("<=================== ARB DBG ==================\n"));

	return 0;
}

static INT32 chip_dump_mib_info(struct hdev_ctrl *ctrl, RTMP_STRING *arg)
{
	RTMP_ADAPTER *pAd = ctrl->priv;
	RTMP_CHIP_CAP *pChipCap = hc_get_chip_cap(pAd->hdev_ctrl);
	UINT8 bss_nums = pChipCap->BssNums;
	UINT32 mac_val, mac_val1, idx, band_idx = 0, band_offset = 0, ampdu_cnt[7] = {0, 0, 0, 0, 0, 0, 0};
	UINT32 msdr6, msdr7, msdr8, msdr9, msdr10, msdr16, msdr17, msdr18, msdr19, msdr20, msdr21;
	UINT32 mbxsdr[bss_nums][4];
	UINT32 mbtcr[16], mbtbcr[16], mbrcr[16], mbrbcr[16];
	UINT32 btcr[bss_nums], btbcr[bss_nums], brcr[bss_nums], brbcr[bss_nums], btdcr[bss_nums], brdcr[bss_nums];
	UINT32 mu_cnt[5];

	for (band_idx = 0; band_idx < pChipCap->band_cnt; band_idx++) {
		if (arg != NULL && band_idx != simple_strtoul(arg, 0, 10))
			continue;

		band_offset = 0x200 * band_idx;
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("Band %d MIB Status\n", band_idx));
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("===============================\n"));
		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_M0SCR0 + band_offset, &mac_val);
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("MIB Status Control=0x%x\n", mac_val));
		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_M0PBSCR + band_offset, &mac_val);
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("MIB Per-BSS Status Control=0x%x\n", mac_val));

		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_M0SDR6 + band_offset, &msdr6);
		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_M0SDR7 + band_offset, &msdr7);
		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_M0SDR8 + band_offset, &msdr8);
		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_M0SDR9 + band_offset, &msdr9);
		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_M0SDR10 + band_offset, &msdr10);
		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_M0SDR16 + band_offset, &msdr16);
		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_M0SDR17 + band_offset, &msdr17);
		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_M0SDR18 + band_offset, &msdr18);
		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_M0SDR19 + band_offset, &msdr19);
		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_M0SDR20 + band_offset, &msdr20);
		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_M0SDR21 + band_offset, &msdr21);

		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("===Phy/Timing Related Counters===\n"));
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tChannelIdleCnt=0x%x\n", msdr6 & 0xffff));
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tCCA_NAV_Tx_Time=0x%x\n", msdr9 & 0xffffff));
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tRx_MDRDY_CNT=0x%x\n", msdr10 & 0x3ffffff));
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			("\tCCK_MDRDY_TIME=0x%x, OFDM_MDRDY_TIME=0x%x, OFDM_GREEN_MDRDY_TIME=0x%x\n",
			msdr19 & 0x3ffffff, msdr20 & 0x3ffffff, msdr21 & 0x3ffffff));
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tPrim CCA Time=0x%x\n", msdr16 & 0xffffff));
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tSec CCA Time=0x%x\n", msdr17 & 0xffffff));
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tPrim ED Time=0x%x\n", msdr18 & 0xffffff));

		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("===Tx Related Counters(Generic)===\n"));
		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_M0SDR0 + band_offset, &mac_val);
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tBeaconTxCnt=0x%x\n", (mac_val & 0xffff)));
		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_M0DR0 + band_offset, &mac_val);
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tTx 20MHz Cnt=0x%x\n", mac_val & 0xffff));
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tTx 40MHz Cnt=0x%x\n", (mac_val >> 16) & 0xffff));
		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_M0DR1 + band_offset, &mac_val);
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tTx 80MHz Cnt=0x%x\n", mac_val & 0xffff));
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tTx 160MHz Cnt=0x%x\n", (mac_val >> 16) & 0xffff));

		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("===AMPDU Related Counters===\n"));
	{
		static UINT32 ampdu_cnt_backup[2][10];
		COUNTER_802_11 *wlanCounter;

		wlanCounter = &pAd->WlanCounters[band_idx];
		if (ampdu_cnt_backup[band_idx][1] > (ampdu_cnt[2] + wlanCounter->AmpduSuccessCount.u.LowPart))
			ampdu_cnt_backup[band_idx][1] = 0;
		ampdu_cnt[2] = (ampdu_cnt[2] + wlanCounter->AmpduSuccessCount.u.LowPart) - ampdu_cnt_backup[band_idx][1];
		ampdu_cnt_backup[band_idx][1] += ampdu_cnt[2];

		if (ampdu_cnt_backup[band_idx][0] > (ampdu_cnt[1] + wlanCounter->AmpduFailCount.u.LowPart + wlanCounter->AmpduSuccessCount.u.LowPart))
			ampdu_cnt_backup[band_idx][0] = 0;
		ampdu_cnt[1] = ampdu_cnt[1] + wlanCounter->AmpduFailCount.u.LowPart + wlanCounter->AmpduSuccessCount.u.LowPart - ampdu_cnt_backup[band_idx][0];
		ampdu_cnt_backup[band_idx][0] += ampdu_cnt[1];

		if (ampdu_cnt_backup[band_idx][2] > (((ampdu_cnt[3]) & 0xffff) + wlanCounter->TxAggRange1Count.u.LowPart))
			ampdu_cnt_backup[band_idx][2] = 0;
		if (ampdu_cnt_backup[band_idx][3] > (((ampdu_cnt[3]>>16) & 0xffff) + wlanCounter->TxAggRange2Count.u.LowPart))
			ampdu_cnt_backup[band_idx][3] = 0;
		ampdu_cnt[3] = (((((ampdu_cnt[3]) & 0xffff) + wlanCounter->TxAggRange1Count.u.LowPart) - ampdu_cnt_backup[band_idx][2]) & 0xffff) |
					   ((((((ampdu_cnt[3] >> 16) & 0xffff) + wlanCounter->TxAggRange2Count.u.LowPart) - ampdu_cnt_backup[band_idx][3]) << 16) & 0xffff0000);
		ampdu_cnt_backup[band_idx][2] += ampdu_cnt[3] & 0xffff;
		ampdu_cnt_backup[band_idx][3] += (ampdu_cnt[3] >> 16) & 0xffff;

		if (ampdu_cnt_backup[band_idx][4] > (((ampdu_cnt[4]) & 0xffff) + wlanCounter->TxAggRange3Count.u.LowPart))
			ampdu_cnt_backup[band_idx][4] = 0;
		if (ampdu_cnt_backup[band_idx][5] > (((ampdu_cnt[4]>>16) & 0xffff) + wlanCounter->TxAggRange4Count.u.LowPart))
			ampdu_cnt_backup[band_idx][5] = 0;
		ampdu_cnt[4] = (((((ampdu_cnt[4]) & 0xffff) + wlanCounter->TxAggRange3Count.u.LowPart) - ampdu_cnt_backup[band_idx][4]) & 0xffff) |
					   ((((((ampdu_cnt[4] >> 16) & 0xffff) + wlanCounter->TxAggRange4Count.u.LowPart) - ampdu_cnt_backup[band_idx][5]) << 16) & 0xffff0000);
		ampdu_cnt_backup[band_idx][4] += ampdu_cnt[4] & 0xffff;
		ampdu_cnt_backup[band_idx][5] += (ampdu_cnt[4] >> 16) & 0xffff;

		if (ampdu_cnt_backup[band_idx][6] > (((ampdu_cnt[5]) & 0xffff) + wlanCounter->TxAggRange5Count.u.LowPart))
			ampdu_cnt_backup[band_idx][6] = 0;
		if (ampdu_cnt_backup[band_idx][7] > (((ampdu_cnt[5]>>16) & 0xffff) + wlanCounter->TxAggRange6Count.u.LowPart))
			ampdu_cnt_backup[band_idx][7] = 0;
		ampdu_cnt[5] = (((((ampdu_cnt[5]) & 0xffff) + wlanCounter->TxAggRange5Count.u.LowPart) - ampdu_cnt_backup[band_idx][6]) & 0xffff) |
					   ((((((ampdu_cnt[5] >> 16) & 0xffff) + wlanCounter->TxAggRange6Count.u.LowPart) - ampdu_cnt_backup[band_idx][7]) << 16) & 0xffff0000);
		ampdu_cnt_backup[band_idx][6] += ampdu_cnt[5] & 0xffff;
		ampdu_cnt_backup[band_idx][7] += (ampdu_cnt[5] >> 16) & 0xffff;

		if (ampdu_cnt_backup[band_idx][8] > (((ampdu_cnt[6]) & 0xffff) + wlanCounter->TxAggRange7Count.u.LowPart))
			ampdu_cnt_backup[band_idx][8] = 0;
		if (ampdu_cnt_backup[band_idx][9] > (((ampdu_cnt[6]>>16) & 0xffff) + wlanCounter->TxAggRange8Count.u.LowPart))
			ampdu_cnt_backup[band_idx][9] = 0;
		ampdu_cnt[6] = (((((ampdu_cnt[6]) & 0xffff) + wlanCounter->TxAggRange7Count.u.LowPart) - ampdu_cnt_backup[band_idx][8]) & 0xffff) |
					   ((((((ampdu_cnt[6] >> 16) & 0xffff) + wlanCounter->TxAggRange8Count.u.LowPart) - ampdu_cnt_backup[band_idx][9]) << 16) & 0xffff0000);
		ampdu_cnt_backup[band_idx][8] += ampdu_cnt[6] & 0xffff;
		ampdu_cnt_backup[band_idx][9] += (ampdu_cnt[6] >> 16) & 0xffff;
	}
		/* MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tRx BA_Cnt=0x%x\n", ampdu_cnt[0] & 0xffff)); */
		/* MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tTx AMPDU_Burst_Cnt=0x%x\n", (ampdu_cnt[0] >> 16 ) & 0xffff)); */
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tTx AMPDU_Pkt_Cnt=0x%x\n", ampdu_cnt[0]));
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tTx AMPDU_MPDU_Pkt_Cnt=0x%x\n", ampdu_cnt[1] & 0xffffff));
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tAMPDU SuccessCnt=0x%x\n", ampdu_cnt[2] & 0xffffff));
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tTx Agg Range: \t1 \t2~5 \t6~15 \t16~22 \t23~33 \t34~49 \t50~57 \t58~64\n"));
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\t\t\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x \t0x%x \t0x%x \t0x%x\n",
			(ampdu_cnt[3]) & 0xffff, (ampdu_cnt[3] >> 16) & 0xffff,
			(ampdu_cnt[4]) & 0xffff, (ampdu_cnt[4] >> 16) & 0xffff,
			(ampdu_cnt[5]) & 0xffff, (ampdu_cnt[5] >> 16) & 0xffff,
			(ampdu_cnt[6]) & 0xffff, (ampdu_cnt[6] >> 16) & 0xffff));

		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("===MU Related Counters===\n"));
		if (band_idx == 1) {
			MAC_IO_READ32(pAd->hdev_ctrl, MIB_M1SDR34, &mu_cnt[0]);
			MAC_IO_READ32(pAd->hdev_ctrl, MIB_M0DR9, &mu_cnt[2]);
			MAC_IO_READ32(pAd->hdev_ctrl, MIB_M0DR8, &mu_cnt[1]);
			MAC_IO_READ32(pAd->hdev_ctrl, MIB_M0DR10, &mu_cnt[3]);
			MAC_IO_READ32(pAd->hdev_ctrl, MIB_M0DR11, &mu_cnt[4]);
		} else {
			memset(mu_cnt, 0x00, sizeof(mu_cnt));
		}
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tMUBF_TX_COUNT=0x%x\n", mu_cnt[0] & 0xffff));
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tMU_TX_MPDU_COUNT(Ok+Fail)=0x%x\n", mu_cnt[1]));
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tMU_TX_OK_MPDU_COUNT=0x%x\n", mu_cnt[2]));
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tMU_TO_SU_PPDU_COUNT=0x%x\n", mu_cnt[3] & 0xffff));
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tSU_TX_OK_MPDU_COUNT=0x%x\n", mu_cnt[4]));

		if (mu_cnt[4] + mu_cnt[1]) {
			MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tMU Ratio=%d %%\n", (mu_cnt[2] * 100)/(mu_cnt[4] + mu_cnt[1])));
		} else {
			MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tMU Ratio=0 %%\n"));
		}

		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("===Rx Related Counters(Generic)===\n"));
		/* MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tVector Overflow Drop Cnt=0x%x\n", (msdr6 >> 16 ) & 0xffff)); */
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tVector Mismacth Cnt=0x%x\n", msdr7 & 0xffff));
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tDelimiter Fail Cnt=0x%x\n", msdr8 & 0xffff));
		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_M0SDR3 + band_offset, &mac_val);
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tRxFCSErrCnt=0x%x\n", (mac_val & 0xffff)));
		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_M0SDR4 + band_offset, &mac_val);
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tRxFifoFullCnt=0x%x\n", (mac_val & 0xffff)));
		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_M0SDR11 + band_offset, &mac_val);
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tRxLenMismatch=0x%x\n", (mac_val & 0xffff)));
		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_M0SDR5 + band_offset, &mac_val);
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tRxMPDUCnt=0x%x\n", (mac_val & 0xffff)));
		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_M0SDR29 + band_offset, &mac_val);
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tPFDropCnt=0x%x\n", (mac_val & 0x00ff)));
		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_M0SDR22 + band_offset, &mac_val);
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tRx AMPDU Cnt=0x%x\n", mac_val));
		/* TODO: shiang-MT7615, is MIB_M0SDR23 used for Rx total byte count for all or just AMPDU only??? */
		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_M0SDR23 + band_offset, &mac_val);
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tRx Total ByteCnt=0x%x\n", mac_val));
	}

	for (idx = 0; idx < bss_nums; idx++) {
		RTMP_IO_READ32(pAd->hdev_ctrl, WTBL_BTCRn + idx * 4, &btcr[idx]);
		RTMP_IO_READ32(pAd->hdev_ctrl, WTBL_BTBCRn + idx * 4, &btbcr[idx]);
		RTMP_IO_READ32(pAd->hdev_ctrl, WTBL_BRCRn + idx * 4, &brcr[idx]);
		RTMP_IO_READ32(pAd->hdev_ctrl, WTBL_BRBCRn + idx * 4, &brbcr[idx]);
		RTMP_IO_READ32(pAd->hdev_ctrl, WTBL_BTDCRn + idx * 4, &btdcr[idx]);
		RTMP_IO_READ32(pAd->hdev_ctrl, WTBL_BRDCRn + idx * 4, &brdcr[idx]);
	}

	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("===Per-BSS Related Tx/Rx Counters===\n"));
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("BSS Idx   TxCnt/DataCnt  TxByteCnt  RxCnt/DataCnt  RxByteCnt\n"));

	for (idx = 0; idx < bss_nums; idx++) {
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("%d\t 0x%x/0x%x\t 0x%x \t 0x%x/0x%x \t 0x%x\n",
			idx, btcr[idx], btdcr[idx], btbcr[idx],
			brcr[idx], brdcr[idx], brbcr[idx]));
	}

	for (idx = 0; idx < bss_nums; idx++) {
		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_MB0SDR0 + idx * 0x10, &mbxsdr[idx][0]);
		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_MB0SDR1 + idx * 0x10, &mbxsdr[idx][1]);
		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_MB0SDR2 + idx * 0x10, &mbxsdr[idx][2]);
		RTMP_IO_READ32(pAd->hdev_ctrl, MIB_MB0SDR3 + idx * 0x10, &mbxsdr[idx][3]);
	}

	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("===Per-MBSS Related MIB Counters===\n"));
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("BSS Idx   RTSTx/RetryCnt  BAMissCnt  AckFailCnt  FrmRetry1/2/3Cnt\n"));

	for (idx = 0; idx < bss_nums; idx++) {
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("%d:\t0x%x/0x%x  0x%x \t 0x%x \t  0x%x/0x%x/0x%x\n",
			idx, mbxsdr[idx][0], (mbxsdr[idx][0] >> 16) & 0xffff,
			mbxsdr[idx][1], (mbxsdr[idx][1] >> 16) & 0xffff,
			mbxsdr[idx][2], (mbxsdr[idx][2] >> 16) & 0xffff,
			mbxsdr[idx][3] & 0xffff));
	}

	for (idx = 0; idx < 16; idx++) {
		RTMP_IO_READ32(pAd->hdev_ctrl, WTBL_MBTCRn + idx * 4, &mbtcr[idx]);
		RTMP_IO_READ32(pAd->hdev_ctrl, WTBL_MBTBCRn + idx * 4, &mbtbcr[idx]);
		RTMP_IO_READ32(pAd->hdev_ctrl, WTBL_MBRCRn + idx * 4, &mbrcr[idx]);
		RTMP_IO_READ32(pAd->hdev_ctrl, WTBL_MBRBCRn + idx * 4, &mbrbcr[idx]);
	}

	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("===Per-MBSS Related Tx/Rx Counters===\n"));
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("MBSSIdx   TxCnt  TxByteCnt  RxCnt  RxByteCnt\n"));

	for (idx = 0; idx < 16; idx++) {
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("%d\t 0x%x\t 0x%x \t 0x%x \t 0x%x\n",
			idx, mbtcr[idx], mbtbcr[idx], mbrcr[idx], mbrbcr[idx]));
	}

	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("===Dummy delimiter insertion result===\n"));
	RTMP_IO_READ32(pAd->hdev_ctrl, MIB_M0DR6, &mac_val);
	RTMP_IO_READ32(pAd->hdev_ctrl, MIB_M0DR7, &mac_val1);
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("Range1 = %d\t Range2 = %d\t Range3 = %d\t Range4 = %d\n",
		GET_TX_DDLMT_RNG1_CNT(mac_val), GET_TX_DDLMT_RNG2_CNT(mac_val), GET_TX_DDLMT_RNG3_CNT(mac_val1), GET_TX_DDLMT_RNG4_CNT(mac_val1)));
#ifdef TRACELOG_TCP_PKT
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("TCP RxAck = %d\t TxData = %d",
		pAd->u4TcpRxAckCnt, pAd->u4TcpTxDataCnt));
	pAd->u4TcpRxAckCnt = 0;
	pAd->u4TcpTxDataCnt = 0;
#endif /* TRACELOG_TCP_PKT */
	return TRUE;
}

static INT32 chip_show_pse_info(struct hdev_ctrl *ctrl, RTMP_STRING *arg)
{
	RTMP_ADAPTER *pAd = ctrl->priv;
	UINT32 pse_buf_ctrl, pg_sz, pg_num;
	UINT32 pse_stat, pg_flow_ctrl[16] = {0};
	UINT32 pse_dbg_stat[9] = {0};
	UINT32 fpg_cnt, ffa_cnt, fpg_head, fpg_tail;
	UINT32 max_q, min_q, rsv_pg, used_pg;
	INT32 i;

	HW_IO_READ32(pAd->hdev_ctrl, PSE_PBUF_CTRL, &pse_buf_ctrl);
	HW_IO_READ32(pAd->hdev_ctrl, PSE_QUEUE_EMPTY, &pse_stat);
	HW_IO_READ32(pAd->hdev_ctrl, PSE_FREEPG_CNT, &pg_flow_ctrl[0]);
	HW_IO_READ32(pAd->hdev_ctrl, PSE_FREEPG_HEAD_TAIL, &pg_flow_ctrl[1]);
	HW_IO_READ32(pAd->hdev_ctrl, PSE_PG_HIF0_GROUP, &pg_flow_ctrl[2]);
	HW_IO_READ32(pAd->hdev_ctrl, PSE_HIF0_PG_INFO, &pg_flow_ctrl[3]);
	HW_IO_READ32(pAd->hdev_ctrl, PSE_PG_HIF1_GROUP, &pg_flow_ctrl[4]);
	HW_IO_READ32(pAd->hdev_ctrl, PSE_HIF1_PG_INFO, &pg_flow_ctrl[5]);
	HW_IO_READ32(pAd->hdev_ctrl, PSE_PG_CPU_GROUP, &pg_flow_ctrl[6]);
	HW_IO_READ32(pAd->hdev_ctrl, PSE_CPU_PG_INFO, &pg_flow_ctrl[7]);
	HW_IO_READ32(pAd->hdev_ctrl, PSE_PG_LMAC0_GROUP, &pg_flow_ctrl[8]);
	HW_IO_READ32(pAd->hdev_ctrl, PSE_LMAC0_PG_INFO, &pg_flow_ctrl[9]);
	HW_IO_READ32(pAd->hdev_ctrl, PSE_PG_LMAC1_GROUP, &pg_flow_ctrl[10]);
	HW_IO_READ32(pAd->hdev_ctrl, PSE_LMAC1_PG_INFO, &pg_flow_ctrl[11]);
	HW_IO_READ32(pAd->hdev_ctrl, PSE_PG_LMAC2_GROUP, &pg_flow_ctrl[12]);
	HW_IO_READ32(pAd->hdev_ctrl, PSE_LMAC2_PG_INFO, &pg_flow_ctrl[13]);
	HW_IO_READ32(pAd->hdev_ctrl, PSE_PG_PLE_GROUP, &pg_flow_ctrl[14]);
	HW_IO_READ32(pAd->hdev_ctrl, PSE_PLE_PG_INFO, &pg_flow_ctrl[15]);
	HW_IO_READ32(pAd->hdev_ctrl, PSE_INT_N9_ERR_STS, &pse_dbg_stat[0]);
	HW_IO_READ32(pAd->hdev_ctrl, PSE_SEEK_CR_00, &pse_dbg_stat[1]);
	HW_IO_READ32(pAd->hdev_ctrl, PSE_SEEK_CR_01, &pse_dbg_stat[2]);
	HW_IO_READ32(pAd->hdev_ctrl, PSE_SEEK_CR_02, &pse_dbg_stat[3]);
	HW_IO_READ32(pAd->hdev_ctrl, PSE_SEEK_CR_03, &pse_dbg_stat[4]);
	HW_IO_READ32(pAd->hdev_ctrl, PSE_SEEK_CR_04, &pse_dbg_stat[5]);
	HW_IO_READ32(pAd->hdev_ctrl, PSE_SEEK_CR_05, &pse_dbg_stat[6]);
	HW_IO_READ32(pAd->hdev_ctrl, PSE_SEEK_CR_06, &pse_dbg_stat[7]);
	HW_IO_READ32(pAd->hdev_ctrl, PSE_SEEK_CR_08, &pse_dbg_stat[8]);
	/* Configuration Info */
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("PSE Configuration Info:\n"));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tPacket Buffer Control(0x82068014): 0x%08x\n", pse_buf_ctrl));
	pg_sz = (pse_buf_ctrl & (0x1 << 31)) >> 31;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tPage Size=%d(%d bytes per page)\n", pg_sz, (pg_sz == 1 ? 256 : 128)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tPage Offset=%d(in unit of 64KB)\n", GET_PSE_PBUF_OFFSET(pse_buf_ctrl)));
	pg_num = PSE_GET_TOTAL_PAGE_CFG(pse_buf_ctrl);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tConfigured Total Page=%d(%d pages)\n", pg_num, (pg_num < 14 ? pse_pg_cnt[pg_num] : 0)));
	pg_num = (pse_buf_ctrl & PSE_TOTAL_PAGE_NUM_MASK);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\t\tAvailable Total Page=%d pages\n", pg_num));
	/* Page Flow Control */
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("PSE Page Flow Control:\n"));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tFree page counter(0x82068100): 0x%08x\n", pg_flow_ctrl[0]));
	fpg_cnt = pg_flow_ctrl[0] & 0xfff;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tThe toal page number of free=0x%03x\n", fpg_cnt));
	ffa_cnt = (pg_flow_ctrl[0] & (0xfff << 16)) >> 16;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tThe free page numbers of free for all=0x%03x\n", ffa_cnt));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tFree page head and tail(0x82068104): 0x%08x\n", pg_flow_ctrl[1]));
	fpg_head = pg_flow_ctrl[1] & 0xfff;
	fpg_tail = (pg_flow_ctrl[1] & (0xfff << 16)) >> 16;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tThe tail/head page of free page list=0x%03x/0x%03x\n", fpg_tail, fpg_head));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tReserved page counter of HIF0 group(0x82068110): 0x%08x\n", pg_flow_ctrl[2]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tHIF0 group page status(0x82068114): 0x%08x\n", pg_flow_ctrl[3]));
	min_q = pg_flow_ctrl[2] & 0xfff;
	max_q = (pg_flow_ctrl[2] & (0xfff << 16)) >> 16;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tThe max/min quota pages of HIF0 group=0x%03x/0x%03x\n", max_q, min_q));
	rsv_pg = pg_flow_ctrl[3] & 0xfff;
	used_pg = (pg_flow_ctrl[3] & (0xfff << 16)) >> 16;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tThe used/reserved pages of HIF0 group=0x%03x/0x%03x\n", used_pg, rsv_pg));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tReserved page counter of HIF1 group(0x82068118): 0x%08x\n", pg_flow_ctrl[4]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tHIF1 group page status(0x8206811c): 0x%08x\n", pg_flow_ctrl[5]));
	min_q = pg_flow_ctrl[4] & 0xfff;
	max_q = (pg_flow_ctrl[4] & (0xfff << 16)) >> 16;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tThe max/min quota pages of HIF1 group=0x%03x/0x%03x\n", max_q, min_q));
	rsv_pg = pg_flow_ctrl[5] & 0xfff;
	used_pg = (pg_flow_ctrl[5] & (0xfff << 16)) >> 16;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tThe used/reserved pages of HIF1 group=0x%03x/0x%03x\n", used_pg, rsv_pg));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tReserved page counter of CPU group(0x82068150): 0x%08x\n", pg_flow_ctrl[6]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tCPU group page status(0x82068154): 0x%08x\n", pg_flow_ctrl[7]));
	min_q = pg_flow_ctrl[6] & 0xfff;
	max_q = (pg_flow_ctrl[6] & (0xfff << 16)) >> 16;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tThe max/min quota pages of CPU group=0x%03x/0x%03x\n", max_q, min_q));
	rsv_pg = pg_flow_ctrl[7] & 0xfff;
	used_pg = (pg_flow_ctrl[7] & (0xfff << 16)) >> 16;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tThe used/reserved pages of CPU group=0x%03x/0x%03x\n", used_pg, rsv_pg));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tReserved page counter of LMAC0 group(0x82068170): 0x%08x\n", pg_flow_ctrl[8]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tLMAC0 group page status(0x82068174): 0x%08x\n", pg_flow_ctrl[9]));
	min_q = pg_flow_ctrl[8] & 0xfff;
	max_q = (pg_flow_ctrl[8] & (0xfff << 16)) >> 16;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tThe max/min quota pages of LMAC0 group=0x%03x/0x%03x\n", max_q, min_q));
	rsv_pg = pg_flow_ctrl[9] & 0xfff;
	used_pg = (pg_flow_ctrl[9] & (0xfff << 16)) >> 16;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tThe used/reserved pages of LMAC0 group=0x%03x/0x%03x\n", used_pg, rsv_pg));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tReserved page counter of LMAC1 group(0x82068178): 0x%08x\n", pg_flow_ctrl[10]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tLMAC1 group page status(0x8206817c): 0x%08x\n", pg_flow_ctrl[11]));
	min_q = pg_flow_ctrl[10] & 0xfff;
	max_q = (pg_flow_ctrl[10] & (0xfff << 16)) >> 16;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tThe max/min quota pages of LMAC1 group=0x%03x/0x%03x\n", max_q, min_q));
	rsv_pg = pg_flow_ctrl[11] & 0xfff;
	used_pg = (pg_flow_ctrl[11] & (0xfff << 16)) >> 16;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tThe used/reserved pages of LMAC1 group=0x%03x/0x%03x\n", used_pg, rsv_pg));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tReserved page counter of LMAC2 group(0x82068180): 0x%08x\n", pg_flow_ctrl[12]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tLMAC2 group page status(0x82068184): 0x%08x\n", pg_flow_ctrl[13]));
	min_q = pg_flow_ctrl[12] & 0xfff;
	max_q = (pg_flow_ctrl[12] & (0xfff << 16)) >> 16;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tThe max/min quota pages of LMAC2 group=0x%03x/0x%03x\n", max_q, min_q));
	rsv_pg = pg_flow_ctrl[13] & 0xfff;
	used_pg = (pg_flow_ctrl[13] & (0xfff << 16)) >> 16;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tThe used/reserved pages of LMAC2 group=0x%03x/0x%03x\n", used_pg, rsv_pg));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tReserved page counter of PLE group(0x82068190): 0x%08x\n", pg_flow_ctrl[14]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tPLE group page status(0x82068194): 0x%08x\n", pg_flow_ctrl[15]));
	min_q = pg_flow_ctrl[14] & 0xfff;
	max_q = (pg_flow_ctrl[14] & (0xfff << 16)) >> 16;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tThe max/min quota pages of PLE group=0x%03x/0x%03x\n", max_q, min_q));
	rsv_pg = pg_flow_ctrl[15] & 0xfff;
	used_pg = (pg_flow_ctrl[15] & (0xfff << 16)) >> 16;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tThe used/reserved pages of PLE group=0x%03x/0x%03x\n", used_pg, rsv_pg));

	/* Misc DBG stat */
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("Misc DBG stat:\n"));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tINT_N9_ERR_STS(0x82068028): 0x%08x\n", pse_dbg_stat[0]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tPSE_SEEK_CR_00(0x820683D0): 0x%08x\n", pse_dbg_stat[1]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tPSE_SEEK_CR_01(0x820683D4): 0x%08x\n", pse_dbg_stat[2]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tPSE_SEEK_CR_02(0x820683D8): 0x%08x\n", pse_dbg_stat[3]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tPSE_SEEK_CR_03(0x820683DC): 0x%08x\n", pse_dbg_stat[4]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tPSE_SEEK_CR_04(0x820683E0): 0x%08x\n", pse_dbg_stat[5]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tPSE_SEEK_CR_05(0x820683E4): 0x%08x\n", pse_dbg_stat[6]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tPSE_SEEK_CR_06(0x820683E8): 0x%08x\n", pse_dbg_stat[7]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tPSE_SEEK_CR_08(0x820683F0): 0x%08x\n", pse_dbg_stat[8]));

	/* Queue Empty Status */
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("PSE Queue Empty Status:\n"));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tQUEUE_EMPTY(0x820680b0): 0x%08x\n", pse_stat));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tCPU Q0/1/2/3 empty=%d/%d/%d/%d\n",
			  pse_stat & 0x1, ((pse_stat & 0x2) >> 1),
			  ((pse_stat & 0x4) >> 2), ((pse_stat & 0x8) >> 3)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tHIF Q0/1 empty=%d/%d\n",
			  ((pse_stat & (0x1 << 16)) >> 16), ((pse_stat & (0x1 << 17)) >> 17)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tLMAC TX Q empty=%d\n",
			  ((pse_stat & (0x1 << 24)) >> 24)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tRLS_Q empty=%d\n",
			  ((pse_stat & (0x1 << 31)) >> 31)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("Nonempty Q info:\n"));

	for (i = 0; i < 31; i++) {
		if (((pse_stat & (0x1 << i)) >> i) == 0) {
			UINT32 hfid, tfid, pktcnt, fl_que_ctrl[3] = {0};

			if (i < 4) {
				MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
						 ("\tCPU Q%d: ", i));
				fl_que_ctrl[0] |= (0x1 << 14);
				fl_que_ctrl[0] |= (i << 8);
			} else if (i == 16) {
				MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tHIF Q0: "));
				fl_que_ctrl[0] |= (0x0 << 14);
				fl_que_ctrl[0] |= (0x0 << 8);
			} else if (i == 17) {
				MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tHIF  Q1: "));
				fl_que_ctrl[0] |= (0x0 << 14);
				fl_que_ctrl[0] |= (0x1 << 8);
			} else if (i == 24) {
				MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tLMAC TX Q: "));
				fl_que_ctrl[0] |= (0x2 << 14);
				fl_que_ctrl[0] |= (0x0 << 8);
			} else if (i == 31) {
				MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tRLS Q: "));
				fl_que_ctrl[0] |= (0x3 << 14);
				fl_que_ctrl[0] |= (i << 8);
			} else
				continue;

			fl_que_ctrl[0] |= (0x1 << 31);
			HW_IO_WRITE32(pAd->hdev_ctrl, PSE_FL_QUE_CTRL_0, fl_que_ctrl[0]);
			HW_IO_READ32(pAd->hdev_ctrl, PSE_FL_QUE_CTRL_2, &fl_que_ctrl[1]);
			HW_IO_READ32(pAd->hdev_ctrl, PSE_FL_QUE_CTRL_3, &fl_que_ctrl[2]);
			hfid = fl_que_ctrl[1] & 0xfff;
			tfid = (fl_que_ctrl[1] & 0xfff << 16) >> 16;
			pktcnt = fl_que_ctrl[2] & 0xfff;
			MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
					 ("tail/head fid = 0x%03x/0x%03x, pkt cnt = %x\n",
					  tfid, hfid, pktcnt));
		}
	}

	return TRUE;
}

static INT32 chip_show_protect_info(struct hdev_ctrl *ctrl, RTMP_STRING *arg)
{
	UINT32 val;
	RTMP_ADAPTER *pAd = ctrl->priv;

	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 (" -Proetction\n"));
	RTMP_IO_READ32(pAd->hdev_ctrl, AGG_PCR, &val);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("  > AGG_PCR 0x%08x\n", val));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 (" -RTS Threshold\n"));
	RTMP_IO_READ32(pAd->hdev_ctrl, AGG_PCR1, &val);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("  > AGG_PCR1 0x%08x\n", val));
	return TRUE;
}

static INT32 chip_show_cca_info(struct hdev_ctrl *ctrl, RTMP_STRING *arg)
{
	UINT32 val;
	RTMP_ADAPTER *pAd = ctrl->priv;

	MAC_IO_READ32(pAd->hdev_ctrl, RMAC_DEBUG_CR, &val);
	val |= (1 << 31); /* For Band0 */
	MAC_IO_WRITE32(pAd->hdev_ctrl, RMAC_DEBUG_CR, val);
	/* Debug CR */
	MAC_IO_WRITE32(pAd->hdev_ctrl, (WF_CFG_OFF_BASE + 0x2c), 0xf);
	MAC_IO_WRITE32(pAd->hdev_ctrl, (WF_CFG_BASE + 0x14), 0x1f);
	MAC_IO_WRITE32(pAd->hdev_ctrl, (WF_CFG_BASE + 0x18), 0x06060606);
	MAC_IO_WRITE32(pAd->hdev_ctrl, (WF_CFG_BASE + 0x4c), 0x1c1c1d1d);
	MAC_IO_READ32(pAd->hdev_ctrl, (WF_CFG_BASE + 0x24), &val);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("CCA for BAND0 info:\n"));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("-- CCA Prim: %d, SE20: %d, SEC40: %d\n",
			  ((val & (1 << 14)) >> 14), ((val & (1 << 6)) >> 6),
			  ((val & (1 << 5)) >> 5)));
	MAC_IO_READ32(pAd->hdev_ctrl, RMAC_DEBUG_CR, &val);
	val &= ~(1 << 31); /* For Band1 */
	MAC_IO_WRITE32(pAd->hdev_ctrl, RMAC_DEBUG_CR, val);
	MAC_IO_READ32(pAd->hdev_ctrl, (WF_CFG_BASE + 0x24), &val);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("CCA for BAND1 info:\n"));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("-- CCA Prim: %d, SE20: %d, SEC40: %d\n",
			  ((val & (1 << 14)) >> 14), ((val & (1 << 6)) >> 6),
			  ((val & (1 << 5)) >> 5)));
	return 0;
}

static INT32 chip_set_cca_en(struct hdev_ctrl *ctrl, RTMP_STRING *arg)
{
	BOOLEAN enable;
	UINT32 val;
	RTMP_ADAPTER *pAd = ctrl->priv;

	enable = os_str_tol(arg, 0, 10);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("Enable CCA on Band0 SEC40: %s\n", (enable) ? "ON" : "OFF"));
	/* RF CR for BAND0 CCA */
	PHY_IO_READ32(pAd->hdev_ctrl, PHY_BAND0_PHY_CCA, &val);
	val |= ((1 << 18) | (1 << 2));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("-- Force Mode: %d, Force CCA SEC40: %d [0x%08x]\n",
			  ((val & (1 << 18)) >> 18), ((val & (1 << 2)) >> 2), val));
	PHY_IO_WRITE32(pAd->hdev_ctrl, PHY_BAND0_PHY_CCA, val);
	/* TMAC_TCR for the normal Tx BW */
	MAC_IO_READ32(pAd->hdev_ctrl, TMAC_TCR, &val);
	val &= ~(PRE_RTS_IDLE_DET_DIS);
	val |= DCH_DET_DIS;
	MAC_IO_WRITE32(pAd->hdev_ctrl, TMAC_TCR, val);
	return TRUE;
}

static INT32 chip_show_dmasch_info(struct hdev_ctrl *ctrl, RTMP_STRING *arg)
{
	RTMP_ADAPTER *pAd = ctrl->priv;
	UINT32 value;
	UINT32 ple_pkt_max_sz;
	UINT32 pse_pkt_max_sz;
	UINT32 max_quota;
	UINT32 min_quota;
	UINT32 rsv_cnt;
	UINT32 src_cnt;
	UINT32 pse_rsv_cnt = 0;
	UINT32 pse_src_cnt = 0;
	UINT32 odd_group_pktin_cnt = 0;
	UINT32 odd_group_ask_cnt = 0;
	UINT32 pktin_cnt;
	UINT32 ask_cnt;
	UINT32 total_src_cnt = 0;
	UINT32 total_rsv_cnt = 0;
	UINT32 ffa_cnt;
	UINT32 free_pg_cnt;
	UINT32 Group_Mapping_Q[16] = {0};
	UINT32 qmapping_addr = MT_HIF_DMASHDL_Q_MAP0;
	UINT32 status_addr = MT_HIF_DMASHDL_STATUS_RD_GP0;
	UINT32 quota_addr = MT_HIF_DMASHDL_GROUP0_CTRL;
	UINT32 pkt_cnt_addr = MT_HIF_DMASHDLRD_GP_PKT_CNT_0;
	UINT32 mapping_mask = 0xf;
	UINT32 mapping_offset = 0;
	UINT32 mapping_qidx;
	UINT32 groupidx = 0;
	UINT8 idx = 0;
	BOOLEAN pktin_int_refill_ena;
	BOOLEAN	pdma_add_int_refill_ena;
	BOOLEAN	ple_add_int_refill_ena;
	BOOLEAN	ple_sub_ena;
	BOOLEAN	hif_ask_sub_ena;
	BOOLEAN	ple_txd_gt_max_size_flag_clr;
	UINT32 ple_rpg_hif;
	UINT32 ple_upg_hif;
	UINT32 pse_rpg_hif = 0;
	UINT32 pse_upg_hif = 0;
	UCHAR is_mismatch = FALSE;

	for (mapping_qidx = 0; mapping_qidx < 32; mapping_qidx++) {
		UINT32 mapping_group;

		idx = 0;

		if (mapping_qidx == 0) {
			qmapping_addr = MT_HIF_DMASHDL_Q_MAP0;
			mapping_mask = 0xf;
			mapping_offset = 0;
		} else if ((mapping_qidx % 8) == 0) {
			qmapping_addr += 0x4;
			mapping_mask = 0xf;
			mapping_offset = 0;
		} else {
			mapping_offset += 4;
			mapping_mask = 0xf << mapping_offset;
		}

		HIF_DMASHDL_IO_READ32(pAd, qmapping_addr, &value);
		mapping_group = (value & mapping_mask) >> mapping_offset;
		Group_Mapping_Q[mapping_group] |= 1 << mapping_qidx;
	}

	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("Dma scheduler info:\n"));
	HIF_DMASHDL_IO_READ32(pAd, MT_HIF_DMASHDL_CTRL_SIGNAL, &value);
	pktin_int_refill_ena = (value & DMASHDL_PKTIN_INT_REFILL_ENA) ? TRUE : FALSE;
	pdma_add_int_refill_ena = (value & DMASHDL_PDMA_ADD_INT_REFILL_ENA) ? TRUE : FALSE;
	ple_add_int_refill_ena = (value & DMASHDL_PLE_ADD_INT_REFILL_ENA) ? TRUE : FALSE;
	ple_sub_ena = (value & DMASHDL_PLE_SUB_ENA) ? TRUE : FALSE;
	hif_ask_sub_ena = (value & DMASHDL_HIF_ASK_SUB_ENA) ? TRUE : FALSE;
	ple_txd_gt_max_size_flag_clr = (value & DMASHDL_PLE_TXD_GT_MAX_SIZE_FLAG_CLR) ? TRUE : FALSE;
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("DMASHDL Ctrl Signal(0x5000A018): 0x%08x\n", value));
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tple_txd_gt_max_size_flag_clr(BIT0) = %d\n", ple_txd_gt_max_size_flag_clr));
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\thif_ask_sub_ena(BIT16) = %d\n", hif_ask_sub_ena));
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tple_sub_ena(BIT17) = %d\n", ple_sub_ena));
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tple_add_int_refill_ena(BIT29) = %d\n", ple_add_int_refill_ena));
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tpdma_add_int_refill_ena(BIT30) = %d\n", pdma_add_int_refill_ena));
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tpktin_int_refill(BIT31)_ena = %d\n", pktin_int_refill_ena));
	HIF_DMASHDL_IO_READ32(pAd, MT_HIF_DMASHDL_PKT_MAX_SIZE, &value);
	ple_pkt_max_sz = GET_PLE_PKT_MAX_SIZE_NUM(value);
	pse_pkt_max_sz = GET_PSE_PKT_MAX_SIZE_NUM(value);
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("DMASHDL Packet_max_size(0x5000A01c): 0x%08x\n", value));
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("PLE/PSE packet max size=0x%03x/0x%03x\n",
			  ple_pkt_max_sz, pse_pkt_max_sz));
	HIF_DMASHDL_IO_READ32(pAd, MT_HIF_DMASHDL_ERROR_FLAG_CTRL, &value);
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("DMASHDL ERR FLAG CTRL(0x5000A09c): 0x%08x\n", value));
	HIF_DMASHDL_IO_READ32(pAd, MT_HIF_DMASHDL_STATUS_RD, &value);
	ffa_cnt = (value & DMASHDL_FFA_CNT_MASK) >> DMASHDL_FFA_CNT_OFFSET;
	free_pg_cnt = (value & DMASHDL_FREE_PG_CNT_MASK) >> DMASHDL_FREE_PG_CNT_OFFSET;
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("DMASHDL Status_RD(0x5000A100): 0x%08x\n", value));
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("free page cnt = 0x%03x, ffa cnt = 0x%03x\n", free_pg_cnt, ffa_cnt));

	for (groupidx = 0; groupidx < 16; groupidx++) {
		idx = 0;
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("Group %d info:", groupidx));
		HIF_DMASHDL_IO_READ32(pAd, status_addr, &value);
		rsv_cnt = (value & DMASHDL_RSV_CNT_MASK) >> DMASHDL_RSV_CNT_OFFSET;
		src_cnt = (value & DMASHDL_SRC_CNT_MASK) >> DMASHDL_SRC_CNT_OFFSET;
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,
				 ("\tDMASHDL Status_RD_GP%d(0x%08x): 0x%08x\n", groupidx, status_addr, value));
		HIF_DMASHDL_IO_READ32(pAd, quota_addr, &value);
		max_quota = (value & DMASHDL_MAX_QUOTA_MASK) >> DMASHDL_MAX_QUOTA_OFFSET;
		min_quota = (value & DMASHDL_MIN_QUOTA_MASK) >> DMASHDL_MIN_QUOTA_OFFSET;
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,
				 ("\tDMASHDL Group%d control(0x%08x): 0x%08x\n", groupidx, quota_addr, value));

		if ((groupidx & 0x1) == 0) {
			HIF_DMASHDL_IO_READ32(pAd, pkt_cnt_addr, &value);
			MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_TRACE,
					 ("\tDMASHDL RD_group_pkt_cnt_%d(0x%08x): 0x%08x\n", groupidx / 2, pkt_cnt_addr, value));
			odd_group_pktin_cnt = GET_ODD_GROUP_PKT_IN_CNT(value);
			odd_group_ask_cnt = GET_ODD_GROUP_ASK_CNT(value);
			pktin_cnt = GET_EVEN_GROUP_PKT_IN_CNT(value);
			ask_cnt = GET_EVEN_GROUP_ASK_CNT(value);
		} else {
			pktin_cnt = odd_group_pktin_cnt;
			ask_cnt = odd_group_ask_cnt;
		}

		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
				 ("\trsv_cnt = 0x%03x, src_cnt = 0x%03x\n", rsv_cnt, src_cnt));
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
				 ("\tmax/min quota = 0x%03x/ 0x%03x\n", max_quota, min_quota));
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
				 ("\tpktin_cnt = 0x%02x, ask_cnt = 0x%02x", pktin_cnt, ask_cnt));

		if (hif_ask_sub_ena && pktin_cnt != ask_cnt) {
			MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, (", mismatch!"));
			is_mismatch = TRUE;
		}

		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\n"));

		if (groupidx == 15 && Group_Mapping_Q[groupidx] == 0) { /* Group15 is for PSE */
			pse_src_cnt = src_cnt;
			pse_rsv_cnt = rsv_cnt;
			break;
		}

		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\tMapping Qidx:"));

		while (Group_Mapping_Q[groupidx] != 0) {
			if (Group_Mapping_Q[groupidx] & 0x1)
				MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
						 ("Q%d ", idx));

			Group_Mapping_Q[groupidx] >>= 1;
			idx++;
		}

		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\n"));
		total_src_cnt += src_cnt;
		total_rsv_cnt += rsv_cnt;
		status_addr = status_addr + 4;
		quota_addr = quota_addr + 4;

		if (groupidx & 0x1)
			pkt_cnt_addr = pkt_cnt_addr + 4;
	}

	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\nCounter Check:\n"));
	MAC_IO_READ32(pAd->hdev_ctrl, PLE_HIF_PG_INFO, &value);
	ple_rpg_hif = value & 0xfff;
	ple_upg_hif = (value & (0xfff << 16)) >> 16;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("PLE:\n\tThe used/reserved pages of PLE HIF group=0x%03x/0x%03x\n",
			  ple_upg_hif, ple_rpg_hif));
	MAC_IO_READ32(pAd->hdev_ctrl, PSE_HIF1_PG_INFO, &value);
	pse_rpg_hif = value & 0xfff;
	pse_upg_hif = (value & (0xfff << 16)) >> 16;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("PSE:\n\tThe used/reserved pages of PSE HIF group=0x%03x/0x%03x\n",
			  pse_upg_hif, pse_rpg_hif));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("DMASHDL:\n\tThe total used pages of group0~14=0x%03x",
			  total_src_cnt));

	if (ple_upg_hif != total_src_cnt) {
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, (", mismatch!"));
		is_mismatch = TRUE;
	}

	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\n"));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tThe total reserved pages of group0~14=0x%03x\n",
			  total_rsv_cnt));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tThe total ffa pages of group0~14=0x%03x\n",
			  ffa_cnt));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tThe total free pages of group0~14=0x%03x",
			  free_pg_cnt));

	if (free_pg_cnt != total_rsv_cnt + ffa_cnt) {
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, (", mismatch(total_rsv_cnt + ffa_cnt in DMASHDL)"));
		is_mismatch = TRUE;
	}

	if (free_pg_cnt != ple_rpg_hif) {
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, (", mismatch(reserved pages in PLE)"));
		is_mismatch = TRUE;
	}

	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\n"));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tThe used pages of group15=0x%03x", pse_src_cnt));

	if (pse_upg_hif != pse_src_cnt) {
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, (", mismatch!"));
		is_mismatch = TRUE;
	}

	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\n"));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tThe reserved pages of group15=0x%03x", pse_rsv_cnt));

	if (pse_rpg_hif != pse_rsv_cnt) {
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, (", mismatch!"));
		is_mismatch = TRUE;
	}

	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\n"));

	if (!is_mismatch)
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("DMASHDL: no counter mismatch\n"));

	return TRUE;
}

static VOID chip_show_bcn_info(struct hdev_ctrl *ctrl, UCHAR bandidx)
{
	RTMP_ADAPTER *pAd = ctrl->priv;
	UINT32 mac_val;
	UINT32 addr;
	UINT32 idx;
	UINT32 band_offset = 0x200 * bandidx;
	struct _RTMP_CHIP_DBG *chip_dbg = hc_get_chip_dbg(ctrl);

	if (chip_dbg->dump_mib_info) {
		if (bandidx == DBDC_BAND0)
			chip_dbg->dump_mib_info(pAd->hdev_ctrl, "0");
		else
			chip_dbg->dump_mib_info(pAd->hdev_ctrl, "1");
	}

	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\n"));
	show_trinfo_proc(pAd, "");
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\n"));
	ShowPLEInfo(pAd, NULL);
#ifdef ERR_RECOVERY
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\n"));
	ShowSerProc2(pAd, "");
#endif
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\n"));
	show_swqinfo(pAd, "");
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\n"));
	if (chip_dbg->show_pse_info)
		chip_dbg->show_pse_info(pAd->hdev_ctrl, NULL);
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\n"));
	show_tpinfo_proc(pAd, NULL);
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\n"));
	Show_MibBucket_Proc(pAd, "");
	MAC_IO_READ32(pAd->hdev_ctrl, ARB_BFCR, &mac_val);
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("ARB_BFCR(0x820f3190)=0x%08x\n", mac_val));
	MAC_IO_READ32(pAd->hdev_ctrl, ARB_SCR, &mac_val);
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("ARB_SCR(0x820f3080)=0x%08x\n", mac_val));
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("dump 0x820f3100~0x820f3154:\n"));

	for (addr = ARB_TQSW0; addr <= ARB_TQPM1; addr = addr + 4) {
		if ((addr & 0xf) == 0 && (addr != ARB_TQSW0))
			MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\n"));

		MAC_IO_READ32(pAd->hdev_ctrl, addr, &mac_val);
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("addr 0x%05x=0x%08x ", addr, mac_val));
	}

	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\n"));
	MAC_IO_READ32(pAd->hdev_ctrl, ARB_BFCR, &mac_val);
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("ARB_BFCR(0x820f3190)=0x%08x\n", mac_val));
	/* (WF_LP) Debug CRs */
	MAC_IO_READ32(pAd->hdev_ctrl, LPON_T0STR, &mac_val);
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("LPON_T0STR(0x820fb028)=0x%08x\n", mac_val));
	MAC_IO_READ32(pAd->hdev_ctrl, LPON_PISR, &mac_val);
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("LPON_PISR(0x820fb030)=0x%08x\n", mac_val));
	MAC_IO_READ32(pAd->hdev_ctrl, LPON_T0TPCR, &mac_val);
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("LPON_T0TPCR(0x820fb34)=0x%08x\n", mac_val));
	MAC_IO_READ32(pAd->hdev_ctrl, LPON_MPTCR0, &mac_val);
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("LPON_MPTCR0(0x820fb05c)=0x%08x\n", mac_val));
	/* (WF_INT_WAKEUP) Interrupt CRs */
	MAC_IO_READ32(pAd->hdev_ctrl, WISR0, &mac_val);
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("WISR0(0x820fc000)=0x%08x\n", mac_val));
	MAC_IO_READ32(pAd->hdev_ctrl, WIER0, &mac_val);
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("WIER0(0x820fc008)=0x%08x\n", mac_val));
	/* (UMAC) CR used to record SER status */
	MAC_IO_READ32(pAd->hdev_ctrl, PSE_SPARE_DUMMY_CR1, &mac_val);
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("SER PSE Error INT status(0x820681e4)=0x%08x\n", mac_val));
	MAC_IO_READ32(pAd->hdev_ctrl, PSE_SPARE_DUMMY_CR2, &mac_val);
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("SER PLE Error INT status(0x820681e8)=0x%08x\n", mac_val));
	MAC_IO_READ32(pAd->hdev_ctrl, PSE_SPARE_DUMMY_CR3, &mac_val);
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("SER LMAC WDT status(0x820682e8)=0x%08x\n", mac_val));
	MAC_IO_READ32(pAd->hdev_ctrl, PP_SPARE_DUMMY_CR5, &mac_val);
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("CritialErrorRecord0(0x8206c064)=0x%08x\n", mac_val));
	MAC_IO_READ32(pAd->hdev_ctrl, PP_SPARE_DUMMY_CR6, &mac_val);
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("CritialErrorRecord1(0x8206c068)=0x%08x\n", mac_val));

#define CR4_HEART_BEAT_STS 0x80200
	MAC_IO_READ32(pAd->hdev_ctrl, CR4_HEART_BEAT_STS, &mac_val);
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("CR4 heart beat status (0x80200)=0x%08x\n", mac_val));
	MtCmdFwLog2Host(pAd, 1, 0);
	MAC_IO_READ32(pAd->hdev_ctrl, RO_BAND0_PHYCTRL_STS + band_offset, &mac_val);
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("RO_BAND0_PHYCTRL_STS(0x%08x)= 0x%08x\n", 0x82070230 + band_offset, mac_val));
	/* 0x82070618~0x8207065c */
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("dump 0x%08x~0x%08x:\n",
			 0x82070618 + band_offset, 0x8207065c + band_offset));

	for (addr = PHY_BAND0_PHYMUX_6; addr <= PHY_BAND0_PHYMUX_23; addr = addr + 4) {
		if ((addr & 0xf) == 8 && (addr != PHY_BAND0_PHYMUX_6))
			MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\n"));

		MAC_IO_READ32(pAd->hdev_ctrl, addr + band_offset, &mac_val);
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("addr 0x%05x=0x%08x ", addr, mac_val));
	}

	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\n"));
	/* 0x8207227c~0x82072294 */
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("dump 0x%08x~0x%08x:\n",
			 0x8207227c + band_offset, 0x82072294 + band_offset));

	for (addr = RO_BAND0_RXTD_DEBUG0; addr <= RO_BAND0_RXTD_DEBUG6; addr = addr + 4) {
		if (addr == RO_BAND0_RXTD_DEBUG4)
			MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\n"));

		MAC_IO_READ32(pAd->hdev_ctrl, addr + band_offset, &mac_val);
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("addr 0x%05x=0x%08x ", addr, mac_val));
	}

	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\n"));
	/* 0x820721a0~0x820721b8 */
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("dump 0x%08x~0x%08x:\n",
			 0x820721a0 + band_offset, 0x820721b8 + band_offset));

	for (addr = RO_BAND0_AGC_DEBUG_0; addr <= RO_BAND0_AGC_DEBUG_6; addr = addr + 4) {
		if (addr == RO_BAND0_AGC_DEBUG_4)
			MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\n"));

		MAC_IO_READ32(pAd->hdev_ctrl, addr + band_offset, &mac_val);
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("addr 0x%05x=0x%08x ", addr, mac_val));
	}

	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\n"));
	/*

		Write 0x82070614[22:20]=0
		Write 0x82070614[22:20]=5

		// read the following registers by 10 times
		0X8207_020C
		0x8207_0210
		0x8207_0214
		0x8207_021C
		0x8207_0220
		// End of Loop

	*/
	MAC_IO_READ32(pAd->hdev_ctrl, PHY_BAND0_PHYMUX_5 + band_offset, &mac_val);
	mac_val &= ~(BITS(20, 22));
	MAC_IO_WRITE32(pAd->hdev_ctrl, PHY_BAND0_PHYMUX_5 + band_offset, mac_val);
	MAC_IO_READ32(pAd->hdev_ctrl, PHY_BAND0_PHYMUX_5 + band_offset, &mac_val);
	mac_val &= ~(BITS(20, 22));
	mac_val |= (BIT(20) | BIT(22));
	MAC_IO_WRITE32(pAd->hdev_ctrl, PHY_BAND0_PHYMUX_5 + band_offset, mac_val);

	for (idx = 0; idx < 10; idx++) {
		if (IS_MT7615(pAd)) {
			MAC_IO_READ32(pAd->hdev_ctrl, RO_BAND0_PHYCTRL_STS0 + band_offset, &mac_val);
			MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("loop %d addr(0x%8x)=0x%08x ",
					 idx, 0x8207020c + band_offset, mac_val));
			MAC_IO_READ32(pAd->hdev_ctrl, RO_BAND0_PHYCTRL_STS1 + band_offset, &mac_val);
			MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("addr(0x%08x)=0x%08x ",
					 0x82070210 + band_offset, mac_val));
			MAC_IO_READ32(pAd->hdev_ctrl, RO_BAND0_PHYCTRL_STS2 + band_offset, &mac_val);
			MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("addr(0x%08x)=0x%08x ",
					 0x82070214 + band_offset, mac_val));
			MAC_IO_READ32(pAd->hdev_ctrl, RO_BAND0_PHYCTRL_STS4 + band_offset, &mac_val);
			MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("addr(0x%08x)=0x%08x ",
					 0x8207021c + band_offset, mac_val));
			MAC_IO_READ32(pAd->hdev_ctrl, RO_BAND0_PHYCTRL_STS5 + band_offset, &mac_val);
			MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("addr(0x%08x)=0x%08x\n",
					 0x82070220 + band_offset, mac_val));
		}
		MAC_IO_READ32(pAd->hdev_ctrl, ARB_BFCR, &mac_val);
		MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("loop %d ARB_BFCR(0x820f3190)=0x%08x\n", idx, mac_val));
	}

	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\n"));
}

static INT32 chip_show_ple_info(struct hdev_ctrl *ctrl, RTMP_STRING *arg)
{
	RTMP_ADAPTER *pAd = ctrl->priv;
	UINT32 ple_buf_ctrl[3] = {0}, pg_sz, pg_num, bit_field_1, bit_field_2;
	UINT32 ple_stat[17] = {0}, pg_flow_ctrl[6] = {0};
	UINT32 sta_pause[4] = {0}, dis_sta_map[4] = {0};
	UINT32 ple_dbg_stat[14] = {0};
	UINT32 fpg_cnt, ffa_cnt, fpg_head, fpg_tail, hif_max_q, hif_min_q;
	UINT32 rpg_hif, upg_hif, cpu_max_q, cpu_min_q, rpg_cpu, upg_cpu;
	INT32 i, j;
	UINT32 dumptxd = 0;

	if (arg != NULL)
		dumptxd = os_str_toul(arg, 0, 16);

	HW_IO_READ32(pAd->hdev_ctrl, PLE_PBUF_CTRL, &ple_buf_ctrl[0]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_RELEASE_CTRL, &ple_buf_ctrl[1]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_HIF_REPORT, &ple_buf_ctrl[2]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_QUEUE_EMPTY, &ple_stat[0]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_AC0_QUEUE_EMPTY_0, &ple_stat[1]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_AC0_QUEUE_EMPTY_1, &ple_stat[2]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_AC0_QUEUE_EMPTY_2, &ple_stat[3]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_AC0_QUEUE_EMPTY_3, &ple_stat[4]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_AC1_QUEUE_EMPTY_0, &ple_stat[5]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_AC1_QUEUE_EMPTY_1, &ple_stat[6]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_AC1_QUEUE_EMPTY_2, &ple_stat[7]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_AC1_QUEUE_EMPTY_3, &ple_stat[8]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_AC2_QUEUE_EMPTY_0, &ple_stat[9]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_AC2_QUEUE_EMPTY_1, &ple_stat[10]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_AC2_QUEUE_EMPTY_2, &ple_stat[11]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_AC2_QUEUE_EMPTY_3, &ple_stat[12]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_AC3_QUEUE_EMPTY_0, &ple_stat[13]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_AC3_QUEUE_EMPTY_1, &ple_stat[14]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_AC3_QUEUE_EMPTY_2, &ple_stat[15]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_AC3_QUEUE_EMPTY_3, &ple_stat[16]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_FREEPG_CNT, &pg_flow_ctrl[0]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_FREEPG_HEAD_TAIL, &pg_flow_ctrl[1]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_PG_HIF_GROUP, &pg_flow_ctrl[2]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_HIF_PG_INFO, &pg_flow_ctrl[3]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_PG_CPU_GROUP, &pg_flow_ctrl[4]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_CPU_PG_INFO, &pg_flow_ctrl[5]);
	HW_IO_READ32(pAd->hdev_ctrl, DIS_STA_MAP0, &dis_sta_map[0]);
	HW_IO_READ32(pAd->hdev_ctrl, DIS_STA_MAP1, &dis_sta_map[1]);
	HW_IO_READ32(pAd->hdev_ctrl, DIS_STA_MAP2, &dis_sta_map[2]);
	HW_IO_READ32(pAd->hdev_ctrl, DIS_STA_MAP3, &dis_sta_map[3]);
	HW_IO_READ32(pAd->hdev_ctrl, STATION_PAUSE0, &sta_pause[0]);
	HW_IO_READ32(pAd->hdev_ctrl, STATION_PAUSE1, &sta_pause[1]);
	HW_IO_READ32(pAd->hdev_ctrl, STATION_PAUSE2, &sta_pause[2]);
	HW_IO_READ32(pAd->hdev_ctrl, STATION_PAUSE3, &sta_pause[3]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_INT_N9_ERR_STS, &ple_dbg_stat[0]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_WF_PLE_PEEK_CR_0, &ple_dbg_stat[1]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_WF_PLE_PEEK_CR_1, &ple_dbg_stat[2]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_WF_PLE_PEEK_CR_2, &ple_dbg_stat[3]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_WF_PLE_PEEK_CR_3, &ple_dbg_stat[4]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_WF_PLE_PEEK_CR_4, &ple_dbg_stat[5]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_WF_PLE_PEEK_CR_5, &ple_dbg_stat[6]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_WF_PLE_PEEK_CR_6, &ple_dbg_stat[7]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_WF_PLE_PEEK_CR_7, &ple_dbg_stat[8]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_WF_PLE_PEEK_CR_8, &ple_dbg_stat[9]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_WF_PLE_PEEK_CR_9, &ple_dbg_stat[10]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_WF_PLE_PEEK_CR_10, &ple_dbg_stat[11]);
	HW_IO_READ32(pAd->hdev_ctrl, PLE_WF_PLE_PEEK_CR_11, &ple_dbg_stat[12]);
	HW_IO_READ32(pAd->hdev_ctrl, WF_PLE_TOP_TP_LMAC_EN, &ple_dbg_stat[13]);
	/* Configuration Info */
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("PLE Configuration Info:\n"));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tPacket Buffer Control(0x82060014): 0x%08x\n", ple_buf_ctrl[0]));
	pg_sz = (ple_buf_ctrl[0] & (0x1 << 31)) >> 31;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tPage Size=%d(%d bytes per page)\n", pg_sz, (pg_sz == 1 ? 128 : 64)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tPage Offset=%d(in unit of 2KB)\n", (ple_buf_ctrl[0] & (0xf << 17)) >> 17));
	pg_num = (ple_buf_ctrl[0] & 0xfff);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tTotal Page=%d pages\n", (ple_buf_ctrl[0] & 0xfff)));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tRelease Control(0x82060030): 0x%08x\n", ple_buf_ctrl[1]));
	bit_field_1 = (ple_buf_ctrl[1] & 0x1f);
	bit_field_2 = ((ple_buf_ctrl[1] & (0x3 << 6)) >> 6);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tNormalTx Release Pid/Qid=%d/%d\n", bit_field_2, bit_field_1));
	bit_field_1 = ((ple_buf_ctrl[1] & (0x1f << 8)) >> 8);
	bit_field_2 = ((ple_buf_ctrl[1] & (0x3 << 14)) >> 14);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tDropTx Release Pid/Qid=%d/%d\n", bit_field_2, bit_field_1));
	bit_field_1 = ((ple_buf_ctrl[1] & (0x1f << 16)) >> 16);
	bit_field_2 = ((ple_buf_ctrl[1] & (0x3 << 22)) >> 22);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tBCN0 Release Pid/Qid=%d/%d\n", bit_field_2, bit_field_1));
	bit_field_1 = ((ple_buf_ctrl[1] & (0x1f << 24)) >> 24);
	bit_field_2 = ((ple_buf_ctrl[1] & (0x3 << 30)) >> 30);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tBCN1 Release Pid/Qid=%d/%d\n", bit_field_2, bit_field_1));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tHIF Report Control(0x82060034): 0x%08x\n", ple_buf_ctrl[2]));
	bit_field_1 = ((ple_buf_ctrl[2] & (0x1 << 1)) >> 1);
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tHostReportQSel/HostReportDisable=%d/%d\n",
			  (ple_buf_ctrl[2] & 0x1), bit_field_1));
	/* Page Flow Control */
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("PLE Page Flow Control:\n"));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tFree page counter(0x82060100): 0x%08x\n", pg_flow_ctrl[0]));
	fpg_cnt = pg_flow_ctrl[0] & 0xfff;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tThe toal page number of free=0x%03x\n", fpg_cnt));
	ffa_cnt = (pg_flow_ctrl[0] & (0xfff << 16)) >> 16;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tThe free page numbers of free for all=0x%03x\n", ffa_cnt));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tFree page head and tail(0x82060104): 0x%08x\n", pg_flow_ctrl[1]));
	fpg_head = pg_flow_ctrl[1] & 0xfff;
	fpg_tail = (pg_flow_ctrl[1] & (0xfff << 16)) >> 16;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tThe tail/head page of free page list=0x%03x/0x%03x\n", fpg_tail, fpg_head));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tReserved page counter of HIF group(0x82060110): 0x%08x\n", pg_flow_ctrl[2]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tHIF group page status(0x82060114): 0x%08x\n", pg_flow_ctrl[3]));
	hif_min_q = pg_flow_ctrl[2] & 0xfff;
	hif_max_q = (pg_flow_ctrl[2] & (0xfff << 16)) >> 16;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tThe max/min quota pages of HIF group=0x%03x/0x%03x\n", hif_max_q, hif_min_q));
	rpg_hif = pg_flow_ctrl[3] & 0xfff;
	upg_hif = (pg_flow_ctrl[3] & (0xfff << 16)) >> 16;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tThe used/reserved pages of HIF group=0x%03x/0x%03x\n", upg_hif, rpg_hif));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tReserved page counter of CPU group(0x82060150): 0x%08x\n", pg_flow_ctrl[4]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\tCPU group page status(0x82060154): 0x%08x\n", pg_flow_ctrl[5]));
	cpu_min_q = pg_flow_ctrl[4] & 0xfff;
	cpu_max_q = (pg_flow_ctrl[4] & (0xfff << 16)) >> 16;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tThe max/min quota pages of CPU group=0x%03x/0x%03x\n", cpu_max_q, cpu_min_q));
	rpg_cpu = pg_flow_ctrl[5] & 0xfff;
	upg_cpu = (pg_flow_ctrl[5] & (0xfff << 16)) >> 16;
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t\tThe used/reserved pages of CPU group=0x%03x/0x%03x\n", upg_cpu, rpg_cpu));

	/* Misc Dbg stat */
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("Misc DBG stat:\n"));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t INT_N9_ERR_STS (0x82060028): 0x%08x\n", ple_dbg_stat[0]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t WF_PLE_PEEK_CR_0 (0x820603D0): 0x%08x\n", ple_dbg_stat[1]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t WF_PLE_PEEK_CR_1 (0x820603D4): 0x%08x\n", ple_dbg_stat[2]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t WF_PLE_PEEK_CR_2 (0x820603D8): 0x%08x\n", ple_dbg_stat[3]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t WF_PLE_PEEK_CR_3 (0x820603DC): 0x%08x\n", ple_dbg_stat[4]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t WF_PLE_PEEK_CR_4 (0x820603E0): 0x%08x\n", ple_dbg_stat[5]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t WF_PLE_PEEK_CR_5 (0x820603E4): 0x%08x\n", ple_dbg_stat[6]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t WF_PLE_PEEK_CR_6 (0x820603E8): 0x%08x\n", ple_dbg_stat[7]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t WF_PLE_PEEK_CR_7 (0x820603EC): 0x%08x\n", ple_dbg_stat[8]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t WF_PLE_PEEK_CR_8 (0x820603F0): 0x%08x\n", ple_dbg_stat[9]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t WF_PLE_PEEK_CR_9 (0x820603F4): 0x%08x\n", ple_dbg_stat[10]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t WF_PLE_PEEK_CR_10 (0x820603F8): 0x%08x\n", ple_dbg_stat[11]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t WF_PLE_PEEK_CR_11 (0x820603FC): 0x%08x\n", ple_dbg_stat[12]));
	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("\t WF_PLE_TOP_TP_LMAC_EN (0x82060220): 0x%08x\n", ple_dbg_stat[13]));

	if (((ple_stat[0] & (0x1 << 24)) >> 24) == 0) {
		MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
				 ("\tAC0_QUEUE_EMPTY0(0x82060300): 0x%08x\n", ple_stat[1]));
		MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
				 ("\tAC1_QUEUE_EMPTY0(0x82060310): 0x%08x\n", ple_stat[5]));
		MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
				 ("\tAC2_QUEUE_EMPTY0(0x82060320): 0x%08x\n", ple_stat[9]));
		MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
				 ("\tAC3_QUEUE_EMPTY0(0x82060330): 0x%08x\n", ple_stat[13]));

		for (j = 0; j < 16; j = j + 4) {
			if (j % 4 == 0) {
				MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
						 ("\n\tNonempty AC%d Q of STA#: ", j / 4));
			}

			for (i = 0; i < 32; i++) {
				if (((ple_stat[j + 1] & (0x1 << i)) >> i) == 0) {
					MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
							 ("%d ", i + (j % 4) * 32));
				}
			}
		}

		MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\n"));
	}

	MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("Nonempty Q info:\n"));

	for (i = 0; i < 31; i++) {
		if (((ple_stat[0] & (0x1 << i)) >> i) == 0) {
			UINT32 hfid, tfid, pktcnt, fl_que_ctrl[3] = {0};

			if (Queue_Empty_info[i].QueueName != NULL) {
				MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF, ("\t%s: ", Queue_Empty_info[i].QueueName));
				fl_que_ctrl[0] |= (0x1 << 31);
				fl_que_ctrl[0] |= (Queue_Empty_info[i].Portid << 14);
				fl_que_ctrl[0] |= (Queue_Empty_info[i].Queueid << 8);
			} else
				continue;

			HW_IO_WRITE32(pAd->hdev_ctrl, PLE_FL_QUE_CTRL_0, fl_que_ctrl[0]);
			HW_IO_READ32(pAd->hdev_ctrl, PLE_FL_QUE_CTRL_2, &fl_que_ctrl[1]);
			HW_IO_READ32(pAd->hdev_ctrl, PLE_FL_QUE_CTRL_3, &fl_que_ctrl[2]);
			hfid = fl_que_ctrl[1] & 0xfff;
			tfid = (fl_que_ctrl[1] & 0xfff << 16) >> 16;
			pktcnt = fl_que_ctrl[2] & 0xfff;
			MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
					 ("tail/head fid = 0x%03x/0x%03x, pkt cnt = %x\n",
					  tfid, hfid, pktcnt));

			if (pktcnt > 0 && dumptxd > 0)
				ShowTXDInfo(pAd, hfid);
		}
	}

	for (j = 0; j < 16; j = j + 4) { /* show AC Q info */
		for (i = 0; i < 32; i++) {
			if (((ple_stat[j + 1] & (0x1 << i)) >> i) == 0) {
				UINT32 hfid, tfid, pktcnt, ac_num = j / 4, ctrl = 0;
				UINT32 sta_num = i + (j % 4) * 32, fl_que_ctrl[3] = {0};
				struct wifi_dev *wdev = wdev_search_by_wcid(pAd, sta_num);
				UINT32 wmmidx = 0;

				if (wdev)
					wmmidx = HcGetWmmIdx(pAd, wdev);

				MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
						 ("\tSTA%d AC%d: ", sta_num, ac_num));
				fl_que_ctrl[0] |= (0x1 << 31);
				fl_que_ctrl[0] |= (0x2 << 14);
				fl_que_ctrl[0] |= (ac_num << 8);
				fl_que_ctrl[0] |= sta_num;
				HW_IO_WRITE32(pAd->hdev_ctrl, PLE_FL_QUE_CTRL_0, fl_que_ctrl[0]);
				HW_IO_READ32(pAd->hdev_ctrl, PLE_FL_QUE_CTRL_2, &fl_que_ctrl[1]);
				HW_IO_READ32(pAd->hdev_ctrl, PLE_FL_QUE_CTRL_3, &fl_que_ctrl[2]);
				hfid = fl_que_ctrl[1] & 0xfff;
				tfid = (fl_que_ctrl[1] & 0xfff << 16) >> 16;
				pktcnt = fl_que_ctrl[2] & 0xfff;
				MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
						 ("tail/head fid = 0x%03x/0x%03x, pkt cnt = %x",
						  tfid, hfid, pktcnt));

				if (((sta_pause[j % 4] & 0x1 << i) >> i) == 1)
					ctrl = 2;

				if (((dis_sta_map[j % 4] & 0x1 << i) >> i) == 1)
					ctrl = 1;

				MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
						 (" ctrl = %s", sta_ctrl_reg[ctrl]));
				MTWF_LOG(DBG_CAT_CFG, DBG_SUBCAT_ALL, DBG_LVL_OFF,
						 (" (wmmidx=%d)\n", wmmidx));

				if (pktcnt > 0 && dumptxd > 0)
					ShowTXDInfo(pAd, hfid);
			}
		}
	}

	return TRUE;
}

#ifdef ERR_RECOVERY
/*
0x820600d0=0x%08X (SER trigger reason)
	BIT(0) //PSE Error Trigger SER
	BIT(1) //PLE Error Trigger SER
	BIT(2) //PP Error Trigger SER
	BIT(3) //LMAC L1 Error Trigger SER
	BIT(4) //LMAC L3 RX Error Trigger SER
	BIT(5) /*LMAC L3 RX Abort Fail
	BIT(6) //LMAC L3 TX0 Error Trigger SER
	BIT(7) //LMAC L3 TX1 Error Trigger SER
	BIT(8) //LMAC BF Error Trigger SER
	BIT(9) //LMAC BF Recovery Fail and Perform L1 Recovery
	BIT(10)/*LMAC L3 continue trigger SER_L3_CONTINUE_TRIGGER_THRES Times
	BIT(11) //DMASHDL Error Trigger SER
*/
static INT32 chip_show_ser_info(struct hdev_ctrl *ctrl)
{
	RTMP_ADAPTER *pAd = ctrl->priv;
	UINT32 reg_tmp_val;

	MAC_IO_READ32(pAd->hdev_ctrl, PLE_BASE+0xd0, &reg_tmp_val);
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("%s,::E  R	, 0x820600d0=0x%08X (SER trigger reason)\n", __func__, reg_tmp_val));
	MAC_IO_READ32(pAd->hdev_ctrl, PLE_BASE+0xd4, &reg_tmp_val);
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("%s,::E  R	, 0x820600d4=0x%08X (PLE error)\n", __func__, reg_tmp_val));
	MAC_IO_READ32(pAd->hdev_ctrl, PLE_BASE+0xd8, &reg_tmp_val);
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("%s,::E  R	, 0x820600d8=0x%08X (PSE error)\n", __func__, reg_tmp_val));
	MAC_IO_READ32(pAd->hdev_ctrl, PSE_BASE+0x1e4, &reg_tmp_val);
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("%s,::E  R	, 0x820681e4=0x%08X (LMAC error)\n", __func__, reg_tmp_val));
	MAC_IO_READ32(pAd->hdev_ctrl, PSE_BASE+0x2ec, &reg_tmp_val);
	MTWF_LOG(DBG_CAT_ALL, DBG_SUBCAT_ALL, DBG_LVL_OFF,
			 ("%s,::E  R	, 0x820682ec=0x%08X (PP/DMASCHDL error)\n", __func__, reg_tmp_val));
}
#endif

VOID mt7626_chip_dbg_init(struct _RTMP_CHIP_DBG *dbg_ops)
{
	dbg_ops->dump_ps_table = chip_dump_ps_table;
	dbg_ops->dump_mib_info = chip_dump_mib_info;
	dbg_ops->show_tmac_info = chip_show_tmac_info;
	dbg_ops->show_agg_info = chip_show_agg_info;
	dbg_ops->show_arb_info = chip_show_arb_info;
	dbg_ops->show_dmasch_info = chip_show_dmasch_info;
	dbg_ops->show_pse_info = chip_show_pse_info;
	dbg_ops->show_pse_data = NULL; /* read PSE data from host is not supported */
	dbg_ops->show_protect_info = chip_show_protect_info;
	dbg_ops->show_cca_info = chip_show_cca_info;
	dbg_ops->set_cca_en = chip_set_cca_en;
	dbg_ops->show_txv_info = NULL;
	dbg_ops->show_bcn_info = chip_show_bcn_info;
	dbg_ops->show_ple_info = chip_show_ple_info;
#ifdef ERR_RECOVERY
	dbg_ops->show_ser_info = chip_show_ser_info;
#endif
}

