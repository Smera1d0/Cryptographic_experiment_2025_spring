# PRESENT 算法的积分攻击实现
# 实现基于对 4 轮的区分器，进而恢复 5 轮密钥

import numpy as np
from random import randint

# PRESENT 的 S 盒和逆 S 盒
SBOX = [0xC, 0x5, 0x6, 0xB, 0x9, 0x0, 0xA, 0xD, 0x3, 0xE, 0xF, 0x8, 0x4, 0x7, 0x1, 0x2]
ISBOX = [0x5, 0xE, 0xF, 0x8, 0xC, 0x1, 0x2, 0xD, 0xB, 0x4, 0x6, 0x3, 0x0, 0x7, 0x9, 0xA]

# PRESENT 的置换表 - 指明每个比特的目标位置
PBOX = [
    0, 16, 32, 48, 1, 17, 33, 49, 2, 18, 34, 50, 3, 19, 35, 51,
    4, 20, 36, 52, 5, 21, 37, 53, 6, 22, 38, 54, 7, 23, 39, 55,
    8, 24, 40, 56, 9, 25, 41, 57, 10, 26, 42, 58, 11, 27, 43, 59,
    12, 28, 44, 60, 13, 29, 45, 61, 14, 30, 46, 62, 15, 31, 47, 63
]

# PRESENT 的逆置换表
IPBOX = [0] * 64
for i in range(64):
    IPBOX[PBOX[i]] = i

class Present:
    def __init__(self, key, rounds=5):
        """初始化 PRESENT 算法对象
        
        Args:
            key: 80 比特或 128 比特密钥（以整数表示）
            rounds: 加密轮数，默认为 32
        """
        self.rounds = rounds
        self.key_size = len(bin(key)) - 2  # 去掉 "0b" 前缀
        
        # 生成轮密钥
        self.round_keys = self.generate_round_keys(key)
    
    def generate_round_keys(self, key):
        """生成轮密钥 - 实现标准 PRESENT 密钥调度算法
        
        Args:
            key: 80 比特主密钥
        
        Returns:
            轮密钥列表，每个元素为 64 比特
        """
        # 对于 80 比特密钥的 PRESENT 标准密钥调度
        round_keys = []
        
        # 初始化 80 位密钥寄存器
        key_register = key
        
        for i in range(self.rounds + 1):
            # 当前轮密钥是密钥寄存器的左侧（最高有效位）64位
            current_round_key = (key_register >> 16) & ((1 << 64) - 1)
            round_keys.append(current_round_key)
            
            # 密钥更新
            # 1. 将寄存器循环左移61位
            key_register = ((key_register << 61) | (key_register >> 19)) & ((1 << 80) - 1)
            
            # 2. 将最左侧4位通过S盒置换
            left_most = (key_register >> 76) & 0xF
            key_register = (key_register & ~(0xF << 76)) | (SBOX[left_most] << 76)
            
            # 3. 将轮计数器与寄存器的特定位异或
            key_register ^= ((i + 1) & 0x1F) << 15
        
        return round_keys
    
    def add_round_key(self, state, round_key):
        """密钥加操作
        
        Args:
            state: 当前状态
            round_key: 轮密钥
        
        Returns:
            密钥加后的状态
        """
        return state ^ round_key
    
    def sbox_layer(self, state):
        """S 盒替代层
        
        Args:
            state: 64 比特状态值
        
        Returns:
            经过 S 盒替代后的状态
        """
        result = 0
        for i in range(16):  # PRESENT 有 16 个 4 比特的 S 盒
            nibble = (state >> (i * 4)) & 0xF
            result |= SBOX[nibble] << (i * 4)
        return result
    
    def inv_sbox_layer(self, state):
        """逆 S 盒替代层
        
        Args:
            state: 64 比特状态值
        
        Returns:
            经过逆 S 盒替代后的状态
        """
        result = 0
        for i in range(16):  # PRESENT 有 16 个 4 比特的 S 盒
            nibble = (state >> (i * 4)) & 0xF
            result |= ISBOX[nibble] << (i * 4)
        return result
    
    def pbox_layer(self, state):
        """置换层
        
        Args:
            state: 64 比特状态值
        
        Returns:
            经过置换后的状态
        """
        result = 0
        for i in range(64):
            if state & (1 << i):
                result |= 1 << PBOX[i]
        return result
    
    def inv_pbox_layer(self, state):
        """逆置换层
        
        Args:
            state: 64 比特状态值
        
        Returns:
            经过逆置换后的状态
        """
        result = 0
        for i in range(64):
            if state & (1 << i):
                result |= 1 << IPBOX[i]
        return result
    
    def encrypt(self, plaintext, rounds=None):
        """加密函数
        
        Args:
            plaintext: 64 比特明文
            rounds: 指定加密轮数，默认为初始化时的轮数
        
        Returns:
            64 比特密文
        """
        if rounds is None:
            rounds = self.rounds
            
        state = plaintext
        
        for i in range(rounds):
            state = self.add_round_key(state, self.round_keys[i])
            state = self.sbox_layer(state)
            if i < rounds - 1:  # 最后一轮不做置换
                state = self.pbox_layer(state)
        
        # 最后一轮密钥加
        state = self.add_round_key(state, self.round_keys[rounds])
        
        return state
    
    def decrypt(self, ciphertext, rounds=None):
        """解密函数
        
        Args:
            ciphertext: 64 比特密文
            rounds: 指定解密轮数，默认为初始化时的轮数
        
        Returns:
            64 比特明文
        """
        if rounds is None:
            rounds = self.rounds
            
        state = ciphertext
        
        # 先做最后一轮密钥加
        state = self.add_round_key(state, self.round_keys[rounds])
        
        for i in range(rounds-1, -1, -1):
            if i < rounds - 1:  # 第一轮解密不做逆置换
                state = self.inv_pbox_layer(state)
            state = self.inv_sbox_layer(state)
            state = self.add_round_key(state, self.round_keys[i])
        
        return state

def integral_distinguisher(key):
    """PRESENT 4 轮积分区分器
    
    Args:
        key: 用于加密的密钥
    
    Returns:
        如果满足平衡性质，返回 True
    """
    cipher = Present(key, rounds=4)
    
    # 创建积分组合，让第一个字节（4 位）取遍所有可能值，其他位保持不变
    plaintexts = []
    base_pt = randint(0, (1 << 64) - 1) & ~0xF  # 清除最低 4 位
    
    for val in range(16):
        plaintexts.append(base_pt | val)
    
    # 加密所有明文
    ciphertexts = [cipher.encrypt(pt, 4) for pt in plaintexts]
    
    # 计算每个输出字节的异或和
    xor_sums = [0] * 16
    for ct in ciphertexts:
        for i in range(16):
            nibble = (ct >> (i * 4)) & 0xF
            xor_sums[i] ^= nibble
    
    # 检查平衡性：所有位置的异或和应为 0
    return all(x == 0 for x in xor_sums)

def integral_distinguisher2(key):
    """另一个 PRESENT 4 轮积分区分器，使用不同的活跃位置
    
    Args:
        key: 用于加密的密钥
    
    Returns:
        如果满足平衡性质，返回 True
    """
    cipher = Present(key, rounds=4)
    
    # 创建积分组合，让第二个字节（4-7位）取遍所有可能值，其他位保持不变
    plaintexts = []
    base_pt = randint(0, (1 << 64) - 1) & ~0xF0  # 清除第二个4位组(4-7位)
    
    for val in range(16):
        plaintexts.append(base_pt | (val << 4))
    
    # 加密所有明文
    ciphertexts = [cipher.encrypt(pt, 4) for pt in plaintexts]
    
    # 计算每个输出字节的异或和
    xor_sums = [0] * 16
    for ct in ciphertexts:
        for i in range(16):
            nibble = (ct >> (i * 4)) & 0xF
            xor_sums[i] ^= nibble
    
    # 检查平衡性：所有位置的异或和应为 0
    return all(x == 0 for x in xor_sums)

def integral_distinguisher3(key):
    """使用第三个字节(8-11位)作为活跃字节的积分区分器"""
    cipher = Present(key, rounds=4)
    
    # 创建积分组合，让第三个字节(8-11位)取遍所有可能值
    plaintexts = []
    base_pt = randint(0, (1 << 64) - 1) & ~0xF00  # 清除第三个4位组
    
    for val in range(16):
        plaintexts.append(base_pt | (val << 8))
    
    # 加密所有明文
    ciphertexts = [cipher.encrypt(pt, 4) for pt in plaintexts]
    
    # 计算异或和
    xor_sums = [0] * 16
    for ct in ciphertexts:
        for i in range(16):
            nibble = (ct >> (i * 4)) & 0xF
            xor_sums[i] ^= nibble
    
    return all(x == 0 for x in xor_sums)



def integral_distinguisher4(key):
    """使用第四个字节(12-15位)作为活跃字节的积分区分器"""
    cipher = Present(key, rounds=4)
    
    # 创建积分组合，让第四个字节取遍所有可能值
    plaintexts = []
    base_pt = randint(0, (1 << 64) - 1) & ~0xF000  # 清除第四个4位组
    
    for val in range(16):
        plaintexts.append(base_pt | (val << 12))
    
    # 加密所有明文
    ciphertexts = [cipher.encrypt(pt, 4) for pt in plaintexts]
    
    # 计算异或和
    xor_sums = [0] * 16
    for ct in ciphertexts:
        for i in range(16):
            nibble = (ct >> (i * 4)) & 0xF
            xor_sums[i] ^= nibble
    
    return all(x == 0 for x in xor_sums)

def key_recovery(ciphertexts):
    """通过积分攻击恢复第 5 轮密钥
    
    Args:
        ciphertexts: 5 轮加密后的密文集合
    
    Returns:
        可能的最后一轮密钥候选
    """
    key_candidates = []
    
    # 对每个 4 位 S 盒位置进行分析
    for pos in range(16):
        valid_keys = []
        
        # 尝试该位置的所有可能密钥值 (4 位)
        for key_guess in range(16):
            xor_sum = 0
            
            # 对每个密文部分解密
            for ct in ciphertexts:
                # 提取对应位置的 4 位
                ct_nibble = (ct >> (pos * 4)) & 0xF
                
                # 解密最后一轮 S 盒
                decrypted = ISBOX[ct_nibble ^ key_guess]
                
                # 累积异或
                xor_sum ^= decrypted
            
            # 如果异或和为 0，这可能是正确的密钥
            if xor_sum == 0:
                valid_keys.append(key_guess)
        
        key_candidates.append(valid_keys)
    
    print("密钥候选：")
    for i, candidates in enumerate(key_candidates):
        print(f"位置 {i}: {[hex(k)[2:] for k in candidates]}")
    
    return key_candidates

def key_recovery_multiple(ciphertext_groups):
    """通过多组积分攻击恢复第 5 轮密钥
    
    Args:
        ciphertext_groups: 多组5轮加密后的密文集合
    
    Returns:
        可能的最后一轮密钥候选（取交集后）
    """
    # 初始化候选密钥列表，每个位置包含所有可能的密钥(0-15)
    final_candidates = [list(range(16)) for _ in range(16)]
    
    # 对每组密文进行分析
    for group_idx, ciphertexts in enumerate(ciphertext_groups):
        print(f"分析第 {group_idx+1} 组密文...")
        group_candidates = []
        
        # 对每个 4 位 S 盒位置进行分析
        for pos in range(16):
            valid_keys = []
            
            # 尝试该位置的所有可能密钥值 (4 位)
            for key_guess in range(16):
                xor_sum = 0
                
                # 对每个密文部分解密
                for ct in ciphertexts:
                    # 提取对应位置的 4 位
                    ct_nibble = (ct >> (pos * 4)) & 0xF
                    
                    # 解密最后一轮 S 盒
                    decrypted = ISBOX[ct_nibble ^ key_guess]
                    
                    # 累积异或
                    xor_sum ^= decrypted
                
                # 如果异或和为 0，这可能是正确的密钥
                if xor_sum == 0:
                    valid_keys.append(key_guess)
            
            group_candidates.append(valid_keys)
        
        # 显示当前组的结果
        print(f"第 {group_idx+1} 组密钥候选：")
        for i, candidates in enumerate(group_candidates):
            print(f"位置 {i}: {[hex(k)[2:] for k in candidates]}")
        
        # 与之前的结果取交集
        for pos in range(16):
            final_candidates[pos] = [k for k in final_candidates[pos] if k in group_candidates[pos]]
    
    print("取交集后的密钥候选：")
    for i, candidates in enumerate(final_candidates):
        print(f"位置 {i}: {[hex(k)[2:] for k in candidates]}")
    
    return final_candidates


def exhaustive_search(key_candidates, ciphertexts, plaintexts):
    """穷举所有可能的密钥组合
    
    Args:
        key_candidates: 每个位置的密钥候选列表
        ciphertexts: 测试加密的密文列表
        plaintexts: 对应的明文列表
    
    Returns:
        找到的有效密钥
    """
    # 统计每个位置的候选密钥数量
    candidate_count = 1
    for nibble_candidates in key_candidates:
        candidate_count *= len(nibble_candidates)
    
    print(f"总共有 {candidate_count} 个可能的密钥组合")
    
    # 如果组合太多，返回最可能的密钥（每个位置选第一个候选）
    if candidate_count > 1073741824:
        print("组合数量过多，返回一个最可能的密钥")
        key = 0
        for i, candidates in enumerate(key_candidates):
            if candidates:
                key |= candidates[0] << (i * 4)
        return key
    
    def try_key_combination(pos=0, current_key=0):
        # 如果所有位置都填满，测试当前密钥
        if pos == 16:
            # 创建一个临时的5轮PRESENT实例，只用于最后一轮
            temp_cipher = Present(0, rounds=5)  # 主密钥无关紧要，我们只替换最后一轮密钥
            temp_cipher.round_keys[5] = current_key
            
            # 测试几个明密文对以提高效率
            test_count = min(5, len(plaintexts))
            for i in range(test_count):
                
                # 或者更简单的方法：重新加密明文比较密文
                encrypted = Present(0, rounds=4).encrypt(plaintexts[i], 4)  # 前4轮加密
                encrypted = temp_cipher.sbox_layer(encrypted)
                encrypted = temp_cipher.pbox_layer(encrypted)
                encrypted = temp_cipher.add_round_key(encrypted, current_key)
                
                if encrypted != ciphertexts[i]:
                    return None
            
            # 所有测试通过，返回找到的密钥
            return current_key
        
        # 尝试当前位置所有可能的密钥值
        for key_nibble in key_candidates[pos]:
            result = try_key_combination(pos + 1, current_key | (key_nibble << (pos * 4)))
            if result is not None:
                return result
        
        return None
    
    # 开始穷举
    print("开始穷举搜索...")
    result = try_key_combination()
    
    if result is None:
        print("找不到有效的密钥")
        # 如果找不到，返回最可能的组合
        key = 0
        for i, candidates in enumerate(key_candidates):
            if candidates:
                key |= candidates[0] << (i * 4)
        return key
    else:
        print("找到有效密钥!")
        return result

def check_key_candidates(original_key, key_candidates):
    """检查正确的密钥是否在候选列表中
    
    Args:
        original_key: 原始密钥（第5轮密钥）
        key_candidates: 取交集后的每个位置的密钥候选列表
    
    Returns:
        正确密钥在候选列表中的位置百分比
    """
    correct_count = 0
    total_positions = len(key_candidates)
    
    print("密钥候选检查结果:")
    for pos in range(total_positions):
        # 获取原始密钥在此位置的值
        correct_nibble = (original_key >> (pos * 4)) & 0xF
        
        # 检查此值是否在候选列表中
        if correct_nibble in key_candidates[pos]:
            correct_count += 1
            status = "√"
        else:
            status = "×"
        
        print(f"位置 {pos}: 正确值 {hex(correct_nibble)[2:]} {status} 候选: {[hex(k)[2:] for k in key_candidates[pos]]}")
    
    success_rate = (correct_count / total_positions) * 100
    print(f"正确密钥包含率: {success_rate:.2f}% ({correct_count}/{total_positions})")
    
    return success_rate

def main():
    """主函数，演示积分攻击流程"""
    # 生成随机 80 比特密钥
    key = randint(0, (1 << 80) - 1)
    
    # 创建 PRESENT 加密对象 - 5 轮
    present = Present(key, rounds=5)
    
    # 测试两个 4 轮积分区分器
    if integral_distinguisher(key):
        print("4 轮积分区分器1验证成功！")
    else:
        print("4 轮积分区分器1验证失败。")
        
    if integral_distinguisher2(key):
        print("4 轮积分区分器2验证成功！")
    else:
        print("4 轮积分区分器2验证失败。")
        
    if integral_distinguisher3(key):
        print("4 轮积分区分器3验证成功！")
    else:
        print("4 轮积分区分器3验证失败。")
        
    if integral_distinguisher4(key):
        print("4 轮积分区分器4验证成功！")
    else:
        print("4 轮积分区分器4验证失败。")
    
    # 为 5 轮密钥恢复创建两组积分组合
    # 第一组：第一个字节活跃
    plaintexts1 = []
    base_pt1 = randint(0, (1 << 64) - 1) & ~0xF  # 清除最低 4 位
    for val in range(16):
        plaintexts1.append(base_pt1 | val)
    
    # 第二组：第二个字节活跃
    plaintexts2 = []
    base_pt2 = randint(0, (1 << 64) - 1) & ~0xF0  # 清除第二个4位组(4-7位)
    for val in range(16):
        plaintexts2.append(base_pt2 | (val << 4))
        
    # 第三组：第三个字节活跃
    plaintexts3 = []
    base_pt3 = randint(0, (1 << 64) - 1) & ~0xF00  # 清除第三个4位组
    for val in range(16):
        plaintexts3.append(base_pt3 | (val << 8))
        
    # 第四组：第四个字节活跃
    plaintexts4 = []
    base_pt4 = randint(0, (1 << 64) - 1) & ~0xF000  # 清除第四个4位组
    for val in range(16):
        plaintexts4.append(base_pt4 | (val << 12))
    
    # 5 轮加密
    ciphertexts1 = [present.encrypt(pt) for pt in plaintexts1]
    ciphertexts2 = [present.encrypt(pt) for pt in plaintexts2]
    ciphertexts3 = [present.encrypt(pt) for pt in plaintexts3]
    ciphertexts4 = [present.encrypt(pt) for pt in plaintexts4]
    
    # 恢复第 5 轮密钥（使用两组密文并取交集）
    key_candidates = key_recovery_multiple([ciphertexts1, ciphertexts2 , ciphertexts3, ciphertexts4])
    # 检查正确的密钥是否在候选列表中
    check_key_candidates(present.round_keys[5], key_candidates)
    
    # 穷举搜索找到完整密钥
    recovered_key = exhaustive_search(key_candidates, ciphertexts1, plaintexts1)
    
    print(f"原始 80 比特密钥: {hex(key)}")
    print(f"原始的第 5 轮密钥: {hex(present.round_keys[5])}")
    print(f"恢复的第 5 轮密钥: {hex(recovered_key)}")
    
    # 验证恢复的密钥
    correct = recovered_key == present.round_keys[5]
    print(f"密钥恢复是否正确: {correct}")
    

    


if __name__ == "__main__":
    main()
    