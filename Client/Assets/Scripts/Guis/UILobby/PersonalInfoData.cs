using UnityEngine;
using BlGame.GameData;
using System.Collections;
using BlGame.Model;

public class PersonalInfoData 
{
    public static PersonalInfoData Instance() {
        if (instance == null) {
            instance = new PersonalInfoData();
        }
        return instance;
    }
    PersonalInfoData() { }

    private static PersonalInfoData instance =null;
    public string gameNick;
    public string gameCount;
    public string gameLevel;
    public string gameWinCount;
    public string gameKillCount;
    public string gameBuildingDestroyCount;
    public string gameDeadCount;
    public string gameMaxAchieve;
    public string gameAchieveCount;
    public string gameExp;
    public string gameMaxExp;
    public string gameHead;
    public string headId;
    
    public void SetCurrentDate(uint headID ,string pszNickName, int UserLv, int UserTotalExp, int UserCurLvExp,
                              int TotalGameInns, int TotalWinInns, int HeroKills, int BuidingDemolishNum,
                              int DeadTimes, int MaxAchNum, int AchNum)
    {
        headId = headID.ToString();
        gameNick = pszNickName;
        gameCount = TotalGameInns.ToString();
        gameLevel = UserLv.ToString();
        gameWinCount = TotalWinInns.ToString();
        gameKillCount = HeroKills.ToString();
        gameBuildingDestroyCount = BuidingDemolishNum.ToString();
        gameDeadCount = DeadTimes.ToString();
        gameAchieveCount = AchNum.ToString();
        gameHead = GameUserModel.Instance.GameUserHead.ToString();
        gameMaxAchieve = MaxAchNum.ToString();
        AchNum = 0;
        if (AchNum <= 0)
        {
            gameAchieveCount = "N";
            gameMaxAchieve = "A";
        }
        if (UserTotalExp <= 0)
        {
            LevelConfigInfo info = ConfigReader.GetLevelInfo(UserLv);
            if (info == null)
                return;
            UserTotalExp = (int)info.LevelUpExp;
        }
        float value = (float)UserCurLvExp / (float)UserTotalExp;
        gameExp = UserCurLvExp.ToString();
        gameMaxExp = UserTotalExp.ToString();
    }
 
}
