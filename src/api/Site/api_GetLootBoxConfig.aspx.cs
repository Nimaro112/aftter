using System;
using System.Collections.Generic;
using System.Data;
using System.Data.SqlClient;
using System.Data.Linq;
using System.Text;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

public partial class api_GetLootBoxConfig : WOApiWebPage
{
    protected override void Execute()
    {
        string skey1 = web.Param("skey1");
        if (skey1 != SERVER_API_KEY)
            throw new ApiExitException("bad key");

        SqlCommand sqcmd = new SqlCommand();
        sqcmd.CommandText = "DBG_GetLootBoxConfig";
        sqcmd.CommandType = CommandType.StoredProcedure;
        if (!CallWOApi(sqcmd))
            return;
        
        StringBuilder result = new StringBuilder();
        result.Append("<?xml version=\"1.0\"?>\n");
        result.Append("<LootBoxDB>\n");

        Dictionary<int, string> lootboxes = new Dictionary<int, string>();
        while (reader.Read())
        {
            StringBuilder xml = new StringBuilder();
            xml.Append("<LootBox ");
            xml.Append(xml_attr("itemID", reader["ItemID"]));
            xml.Append(xml_attr("SrvNormal", reader["SrvNormal"]));
            xml.Append(xml_attr("SrvTrial", reader["SrvTrial"]));
            xml.Append(xml_attr("SrvPremium", reader["SrvPremium"]));
            xml.Append(">\n");
            lootboxes.Add((int)reader["ItemID"], xml.ToString());
        }

        Dictionary<int, string[]> lootboxItems = new Dictionary<int, string[]>();
        sqcmd = new SqlCommand();
        sqcmd.CommandText = "DBG_GetLootBoxItems";
        sqcmd.CommandType = CommandType.StoredProcedure;
        if (!CallWOApi(sqcmd))
            return;
        while (reader.Read())
        {
            //<d c=\"$l_Chance\" i=\"$l_ItemID\" g1=\"$l_GDMin\" g2=\"$l_GDMax\" />
            if (!lootboxes.ContainsKey((int) reader["LootID"]))
                continue;
            StringBuilder innerXml = new StringBuilder(lootboxes[(int)reader["LootID"]]);
            innerXml.Append("<d ");
            innerXml.Append(xml_attr("c", reader["Chance"]));
            innerXml.Append(xml_attr("i", reader["ItemID"]));
            innerXml.Append(xml_attr("g1", reader["GDMin"]));
            innerXml.Append(xml_attr("g2", reader["GDMax"]));
            innerXml.Append("/>\n");
            lootboxes[(int) reader["LootID"]] = innerXml.ToString();
        }

        foreach (KeyValuePair<int, string> val in lootboxes)
        {
            StringBuilder innerXml = new StringBuilder(val.Value);
            innerXml.Append("</LootBox>\n");
            result.Append(innerXml);
        }
        result.Append("</LootBoxDB>\n\n");

        GResponse.Write(result.ToString());
    }
}